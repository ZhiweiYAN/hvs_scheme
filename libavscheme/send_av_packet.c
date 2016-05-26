/*
 * =====================================================================================
 *
 *       Filename:  send_av_packet.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/21/2010 9:59:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "send_av_packet.h"

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_send_av_packet_pthread
 *  Description:
 * =====================================================================================
 */
int Initialize_send_av_packet_pthread(T_Encoded_Av_Packet_Queue *q, T_System_State *sys_state)
{
    assert( NULL!=q);

    int err = 0;
    if (NULL==q) {
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

    T_Send_Thread_Param *param = NULL;
    param = malloc( sizeof(T_Send_Thread_Param) );
    assert(NULL!=param);
    param->worker_id = sys_state->rec_thread_num
			+ sys_state->dec_thread_num
			+ sys_state->mix_thread_num
			+ sys_state->enc_thread_num;
	
    param->encoded_av_pkt_q = q;
    param->sys_state = sys_state;

    err = pthread_create(tid, NULL, (void *)Send_av_packets, (void *) param);

    if (0 != err) {
        OUTPUT_ERROR;
        return -1;
    }
    DBG("-->Sending av packets: the thread (0x%x) is running.\n", (unsigned int)tid );

    free (tid);
    tid = NULL;

    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Send_av_packets
 *  Description:  Fetch a Av_packet node from the queue and send it to its peer
 * =====================================================================================
 */
void *Send_av_packets(T_Send_Thread_Param *param)
{
    assert(NULL!=param);

	int err = 0;

    //backup the input parameters
    T_Send_Thread_Param p;
    memcpy(&p, param, sizeof(T_Send_Thread_Param) );
    pthread_detach( pthread_self() );
    free(param);
	param = NULL;

	T_Encoded_Av_Packet_Queue *encoded_av_pkt_q = NULL;
	encoded_av_pkt_q = p.encoded_av_pkt_q;
	
    //monitor probe
    T_Worker_Info *p_worker_info = NULL;
    p_worker_info = Get_worker_info_pointer(p.sys_state,p.worker_id);
    T_Queue_Info *p_queue_info_av_pkt = NULL;
    p_queue_info_av_pkt = Get_queue_info_pointer(p.sys_state, encoded_av_pkt_q->q_info.id);

    T_Encoded_Av_Packet_Node *n = NULL;

	int pkt_id = 0;
	
//	Dead loop
    for (;;) {

        //Monitor probe
        Set_worker_state(p_worker_info, WAITING_STATE);
        p_queue_info_av_pkt->length = p.encoded_av_pkt_q->q_info.length;

		n = NULL;
        err = Remove_encoded_av_packet_queue(encoded_av_pkt_q, &n);
        if (NULL==n) {
            usleep(10);
            continue;
        }

        DBG("%s(%d)%s(LEN %d,ID %d):||\n","-->Udp Port",0000," Send packet: ", 10000, pkt_id++);
        //Sending Mixed Av packets.
        free(n->av_pkt);
        n->av_pkt = NULL;
        free(n);
        n = NULL;
        //... ...
        //... ...

        //Monitor probe
        Set_worker_state(p_worker_info, TAKENODE_STATE);
        Set_worker_state(p_worker_info, WORKING_STATE);
        Set_worker_state(p_worker_info, COMPLETED_STATE);
    }
}

