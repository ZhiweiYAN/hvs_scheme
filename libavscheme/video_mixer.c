/*
 * =====================================================================================
 *
 *       Filename:  video_mixer.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/16/2010 9:25:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "video_mixer.h"
#include "pseduo_vcc.h"


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_video_mixer_pthread
 *  Description:
 * =====================================================================================
 */
int Initialize_video_mixer_pthread(T_Decoded_Av_Frame_Queue **decoded_av_frm_q,
                                   T_Mixed_Av_Frame_Queue *mixed_av_frm_q, T_System_State *sys_state)
{
    assert(NULL!=decoded_av_frm_q);
    assert(NULL!=mixed_av_frm_q);
    assert(NULL!=sys_state);


    int err = 0;

    if (NULL==decoded_av_frm_q) {
        OUTPUT_ERROR;
        return 0;
    }

    int mix_thread_num = 0;
    mix_thread_num = sys_state->mix_thread_num;

    pthread_t *tid = NULL;
    tid = malloc( (mix_thread_num)*sizeof(pthread_t) );
    if (NULL==tid) {
        OUTPUT_ERROR;
        return 0;
    }
    bzero(tid, (mix_thread_num)*sizeof(pthread_t) );

    for (int i=0; i<mix_thread_num; i++) {

        T_Mixer_Thread_Param *param = NULL;
        param = malloc( sizeof(T_Mixer_Thread_Param) );
        assert( NULL != param );

        param->worker_id = sys_state->rec_thread_num + sys_state->dec_thread_num + i;
        param->decoded_av_frm_qs = decoded_av_frm_q;
        param->mixed_av_frm_qs = mixed_av_frm_q;
        param->sys_state = sys_state;

        err = pthread_create((tid+i), NULL, (void *)Mix_av_frames, (void *) param);

        if (0 != err) {
            OUTPUT_ERROR;
            return -1;
        }
        DBG("-->Mixing av frames: the thread (0x%x) is running.\n", (unsigned int)*(tid+i));
    }
    //Delay for guarratting the all threads have being running.
    free(tid);
    tid = NULL;
    return 1;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Mix_av_frames()
 *  Description:
 * =====================================================================================
 */
void * Mix_av_frames(T_Mixer_Thread_Param *param)
{
    assert(NULL != param);

    int err = 0;

    //backup the input parameters
    T_Mixer_Thread_Param p;
    memcpy( &p, param, sizeof(T_Mixer_Thread_Param) );
    pthread_detach( pthread_self() );
    free(param);
    param = NULL;

    T_Decoded_Av_Frame_Queue **decoded_av_frm_qs = NULL;
    T_Mixed_Av_Frame_Queue *mixed_av_frm_q = NULL;
    decoded_av_frm_qs = p.decoded_av_frm_qs;
    mixed_av_frm_q = p.mixed_av_frm_qs;

    //monitor probe
    T_Worker_Info *p_worker_info = NULL;
    p_worker_info = Get_worker_info_pointer(p.sys_state,p.worker_id);
    T_Queue_Info **p_dec_queue_info_av_frm = NULL;
    p_dec_queue_info_av_frm = malloc ( (p.sys_state->dec_queue_num) * sizeof(void *) );
    for ( int i=0; i<p.sys_state->dec_queue_num; i++) {
        p_dec_queue_info_av_frm[i] = Get_queue_info_pointer(p.sys_state, p.decoded_av_frm_qs[i]->q_info.id);
    }
    T_Queue_Info *p_mix_queue_info_av_frm = NULL;
    p_mix_queue_info_av_frm = Get_queue_info_pointer(p.sys_state, mixed_av_frm_q->q_info.id);

    int frm_n = p.sys_state->dec_queue_num;
    for(int i=0; i<frm_n; i++) {
        assert(NULL!=decoded_av_frm_qs[i]);
    }

    void **frms = NULL;
    frms = malloc( frm_n * sizeof(void *) );
    assert(NULL!=frms);
    bzero( frms, frm_n * sizeof(void*) );

    T_Decoded_Av_Frame_Node **av_frm_nodes = NULL;
    av_frm_nodes = malloc (frm_n *sizeof(T_Decoded_Av_Frame_Node) );
    assert(NULL!=av_frm_nodes);
    bzero( av_frm_nodes, frm_n * sizeof(T_Decoded_Av_Frame_Node) );

    int (*f)() = NULL;
    f = &Insert_mixed_av_frame_queue_wraper;
    int empty_flag = 0;

    for (;;) {

		empty_flag = 0;
        //Monitor probe
        Set_worker_state(p_worker_info, WAITING_STATE);
        for (int i=0; i<frm_n; i++) {
            p_dec_queue_info_av_frm[i]->length = decoded_av_frm_qs[i]->q_info.length;
        }
        p_mix_queue_info_av_frm->length= mixed_av_frm_q->q_info.length;

        //Fetch the n frames from their queues
        for ( int i=0; i<frm_n; i++) {
            av_frm_nodes[i] = NULL;
            frms[i] = NULL;
            err = Remove_decoded_av_frame_queue(decoded_av_frm_qs[i], &(av_frm_nodes[i]) );

            //if there are no nodes (equals -1) in the queue, or else...
            if (NULL!=av_frm_nodes[i]) {
                frms[i] = av_frm_nodes[i]->av_frame;
            } else {
                empty_flag ++;
            }

        }

        //make sure that all frm queues are empty.
        if (frm_n==empty_flag) {
            usleep(10);
            continue;

        }
		
        //Monitor probe
        Set_worker_state(p_worker_info, TAKENODE_STATE);

        //Do the process of mixer
        Pseduo_Do_mix_frames(frms, frm_n, f, mixed_av_frm_q);

        //Monitor probe
        Set_worker_state(p_worker_info, WORKING_STATE);
        Set_worker_state(p_worker_info, PLACENODE_STATE);
        Set_worker_state(p_worker_info, COMPLETED_STATE);

        for (int i=0; i<frm_n; i++) {
            free(frms[i]);
            frms[i] = NULL;
            free(av_frm_nodes[i]);
            av_frm_nodes[i] = NULL;

        }

    }
    return ( (void *)1 );

}
