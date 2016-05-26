/*
 * =====================================================================================
 *
 *       Filename:  video_encoder.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/19/2010 1:33:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "video_encoder.h"
#include "pseduo_vcc.h"
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_video_encoder_pthread
 *  Description:  video encoder
 * =====================================================================================
 */
int Initialize_video_encoder_pthread( T_Mixed_Av_Frame_Queue *mixed_av_frm_q,
                                      T_Encoded_Av_Packet_Queue *encoded_av_pkt_q, T_System_State *sys_state)
{
    assert( NULL!=mixed_av_frm_q);
    assert( NULL!=encoded_av_pkt_q);
    assert( NULL!=sys_state);

    int err = 0;
    if (NULL==mixed_av_frm_q || NULL==encoded_av_pkt_q) {
        OUTPUT_ERROR;
        return 0;
    }

    pthread_t *tid = NULL;

    tid = malloc( sizeof(pthread_t) );
    if (NULL==tid) {
        OUTPUT_ERROR;
        return 0;
    }
    bzero(tid, sizeof(pthread_t) );

    //for the input parameter being transporting into the thread
    T_Encoder_Thread_Param *param = NULL;
    param = malloc( sizeof(T_Encoder_Thread_Param) );
    assert( NULL!= param );
    param->worker_id = sys_state->rec_thread_num
                       + sys_state->dec_thread_num
                       + sys_state->mix_thread_num;
    param->mixed_av_frm_q = mixed_av_frm_q;
    param->encoded_av_pkt_q = encoded_av_pkt_q;
    param->sys_state = sys_state;

    err = pthread_create(tid, NULL, (void *)Encode_Av_Frames, (void *) param);

    if (0 != err) {
        OUTPUT_ERROR;
        return -1;
    }
    DBG("-->Encoding av frames: the thread (0x%x) is running.\n", (unsigned int) *(tid) );

    //Delay for guarratting the all threads have being running.
//    sleep(1);
//    free(param);
//    param = NULL;

    free (tid);
    tid = NULL;

    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Encode_Av_Frames
 *  Description:
 * =====================================================================================
 */
void * Encode_Av_Frames(T_Encoder_Thread_Param *param)
{
    assert(NULL!=param);

    int err = 0;

    //backup the input parameters
    T_Encoder_Thread_Param p;
    memcpy(&p, param, sizeof(T_Encoder_Thread_Param) );
	
    pthread_detach( pthread_self() );
    free(param);
	param = NULL;
		
    T_Mixed_Av_Frame_Queue *mixed_av_frm_q = NULL;
    T_Encoded_Av_Packet_Queue *encoded_av_pkt_q = NULL;
    mixed_av_frm_q = p.mixed_av_frm_q;
    encoded_av_pkt_q  = p.encoded_av_pkt_q;

    //monitor probe
    T_Worker_Info *p_worker_info = NULL;
    p_worker_info = Get_worker_info_pointer(p.sys_state,p.worker_id);
    T_Queue_Info *p_mix_queue_info_av_frm = NULL;
    p_mix_queue_info_av_frm = Get_queue_info_pointer(p.sys_state, mixed_av_frm_q->q_info.id);
    T_Queue_Info *p_enc_queue_info_av_pkt = NULL;
    p_enc_queue_info_av_pkt = Get_queue_info_pointer(p.sys_state, encoded_av_pkt_q->q_info.id);

    int (*f)() = NULL;
    f = &Insert_encoded_av_packet_queue_wraper;
    T_Mixed_Av_Frame_Node *mixed_av_frm_n = NULL;

    for (;;) {

        //Monitor probe
        Set_worker_state(p_worker_info, WAITING_STATE);
        p_mix_queue_info_av_frm->length= mixed_av_frm_q->q_info.length;
        p_enc_queue_info_av_pkt->length = encoded_av_pkt_q->q_info.length;

    	mixed_av_frm_n = NULL;
        err = Remove_mixed_av_frame_queue(mixed_av_frm_q, &mixed_av_frm_n);

        //if there are no nodes (equals -1) in the queue, or else...
        if (NULL==mixed_av_frm_n) {
            usleep(10);
            continue;
        }
/*
		free(mixed_av_frm_n->av_frame);
		mixed_av_frm_n->av_frame = NULL;
		free(mixed_av_frm_n);
		mixed_av_frm_n = NULL;
		continue;
*/		
        //Monitor probe
        Set_worker_state(p_worker_info, TAKENODE_STATE);

        //Encoding Mixed YUV Frames.
        Pseduo_Do_encode_frame(mixed_av_frm_n->av_frame, f, encoded_av_pkt_q);	
		
        //... ...
        //Monitor probe
        Set_worker_state(p_worker_info, PLACENODE_STATE);
        Set_worker_state(p_worker_info, COMPLETED_STATE);

        free(mixed_av_frm_n);
        mixed_av_frm_n = NULL;

    }
}

