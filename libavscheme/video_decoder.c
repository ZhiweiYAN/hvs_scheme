/*
 * =====================================================================================
 *
 *       Filename:  video_decoder.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/15/2010 9:16:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "video_decoder.h"
#include "pseduo_vcc.h"

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_decode_av_packet_pthreads()
 *  Description:
 * =====================================================================================
 */
int Initialize_decode_av_packet_pthreads(T_Recv_Av_Packet_Queue **av_pkt_q,
        T_Decoded_Av_Frame_Queue **av_frm_q, T_System_State *sys_state)
{
    assert(NULL!=av_pkt_q);
    assert(NULL!=av_frm_q);
    assert(NULL!=sys_state);

    int err = 0;

    if (NULL==av_pkt_q || NULL==av_frm_q) {
        OUTPUT_ERROR;
        return 0;
    }

    int n = 0;
    n = sys_state->dec_thread_num;

    pthread_t *ntid = NULL;
    ntid = malloc( n*sizeof(pthread_t) );
    if (NULL==ntid) {
        OUTPUT_ERROR;
        return 0;
    }
    bzero(ntid,  n*sizeof(pthread_t) );

	T_Decoder_Thread_Param *param = NULL;
	
    for (int i=0; i<n; i++) {

        //  Prepare the input parameters that are passed into the threads.
        param = malloc( sizeof(T_Decoder_Thread_Param) );
        //free the memory in the child thread.
        assert(NULL!=param);

        param->worker_id = sys_state->rec_thread_num + i;
        param->av_pkt_q = av_pkt_q[i];
        param->av_frm_q = av_frm_q[i];
        param->sys_state = sys_state;

        //assign the pointers of the i-th queues to the i-th qs
        err = pthread_create(ntid+i, NULL, (void *)Decode_av_packets, (void *)(param));
        if (0 != err) {
            OUTPUT_ERROR;
            return -1;
        }

        DBG("-->Decoding av packets: the thread (0x%x) is running.\n", (unsigned int) *(ntid+i) );

        //usleep((int)(200/n));
        fflush(NULL);
    }

    //Delay for guarratting the all threads have being running.
    //sleep(1);

    free(ntid);
    ntid = NULL;
    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Decode_av_packets
 *  Description:
 * =====================================================================================
 */
void * Decode_av_packets(T_Decoder_Thread_Param *param)
{

    int err = 0;
    assert(NULL!=param);

    if (NULL==param->av_pkt_q||NULL==param->av_frm_q) {
        OUTPUT_ERROR;
        return ((void *) 0);
    }

    //backup the input parameters
    T_Decoder_Thread_Param p;
    memcpy(&p, param, sizeof(T_Decoder_Thread_Param) );

    pthread_detach( pthread_self() );
    free(param);
	param = NULL;
    T_Recv_Av_Packet_Queue *av_pkt_q = NULL;
    T_Decoded_Av_Frame_Queue *av_frm_q = NULL;

    av_pkt_q = p.av_pkt_q;
    av_frm_q = p.av_frm_q;
    T_Recv_Av_Packet_Node *av_pkt_node = NULL;

    //monitor probe
    T_Worker_Info *p_worker_info = NULL;
    p_worker_info = Get_worker_info_pointer(p.sys_state,p.worker_id);
    T_Queue_Info *p_queue_info_av_pkt = NULL;
    p_queue_info_av_pkt = Get_queue_info_pointer(p.sys_state, av_pkt_q->q_info.id);
    T_Queue_Info *p_queue_info_av_frm = NULL;
    p_queue_info_av_frm = Get_queue_info_pointer(p.sys_state, av_frm_q->q_info.id);

    int (*f)() = NULL;
    f = &Insert_decoded_av_frame_queue_wraper;

    //Dead loop
    for (;;) {

        //Monitor probe
        Set_worker_state(p_worker_info, WAITING_STATE);
        p_queue_info_av_pkt->length = av_pkt_q->q_info.length;
        p_queue_info_av_frm->length = av_frm_q->q_info.length;

        //Remove the a node of AV packet from the AV pakcet queue
        av_pkt_node = NULL;
        err = Remove_recv_av_packet_queue(av_pkt_q, &av_pkt_node);

        //if there are no nodes (equals -1) in the queue, or else...
        if (NULL==av_pkt_node ) {
            usleep(10);
            continue;
        }
/*		
		free( av_pkt_node->av_pkt );
		av_pkt_node->av_pkt = NULL;
		free(av_pkt_node);
		av_pkt_node = NULL;
		continue;
*/		

        //Monitor probe
        Set_worker_state(p_worker_info, TAKENODE_STATE);
        Set_worker_state(p_worker_info, WORKING_STATE);

        //Add video decoding codes here
        //.....
        Pseduo_Do_decode_packet( av_pkt_node->av_pkt, f, av_frm_q);

        //Monitor probe
        Set_worker_state(p_worker_info, PLACENODE_STATE);
        Set_worker_state(p_worker_info, COMPLETED_STATE);

        //free the memory of the av packet
        free(av_pkt_node);
        av_pkt_node = NULL;
    }//dead loop end
}

