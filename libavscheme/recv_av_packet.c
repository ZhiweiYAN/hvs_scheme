/*
 * =====================================================================================
 *
 *       Filename:  recv_av_packet.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/13/2010 8:55:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "recv_av_packet.h"
#define  MAX_SIZE_BUFFER_RECV_T 2000000

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_recv_av_packet_pthreads()
 *  Description:  Create multiple threads of receiving av packets
 * =====================================================================================
 */
int Initialize_recv_av_packet_pthreads(T_Recv_Av_Packet_Queue **qs, T_System_State *sys_state)
{
    assert(NULL!=qs);
    assert(NULL!=sys_state);

    int err;

    if (NULL==qs||NULL==sys_state) {
        OUTPUT_ERROR;
        return 0;
    }

    int n = 0;
    n = sys_state->rec_queue_num;

    pthread_t *ntid = NULL;
    ntid = malloc( n*sizeof(pthread_t) );
    if (NULL==ntid) {
        OUTPUT_ERROR;
        return 0;
    }

    T_Recv_Thread_Param *param = NULL;
    for (int i=0; i<n; i++) {

        param = malloc( sizeof(T_Recv_Thread_Param) );
        assert( NULL!= param );
        (param)->worker_id = i;
        (param)->q = qs[i];
        (param)->sys_state = sys_state;

        err = pthread_create(ntid+i, NULL, (void *)Recv_av_packets, (void *)(param));
        if (0 != err) {
            OUTPUT_ERROR;
            return -1;
        }
//       usleep((int)(1000/n));
        DBG("-->Recving av packets: the thread (0x%x) is running.\n", (unsigned int)*(ntid+i) );

        fflush(NULL);
    }
//    sleep(1);
    free(ntid);
    ntid = NULL;
    return 1;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  recv_av_packets
 *  Description:  Opens upd port, and then it enters the dead loop to receive the av
 *                packets which will be inserted into a queue.
 * =====================================================================================
 */
void * Recv_av_packets(T_Recv_Thread_Param *param)
{

    assert(NULL!=param);

    int err = 0;
    int pkt_id = 0;

    //backup the input parameters
    T_Recv_Thread_Param p;
    memcpy(&p, param, sizeof(T_Recv_Thread_Param) );

    pthread_detach( pthread_self() );
    free(param);
    param = NULL;

    T_Recv_Av_Packet_Queue *recv_av_pkt_q = NULL;
    recv_av_pkt_q = p.q;

    //monitor probe
    T_Worker_Info *p_worker_info = NULL;
    p_worker_info = Get_worker_info_pointer(p.sys_state,p.worker_id);
    T_Queue_Info *p_queue_info_av_pkt = NULL;
    p_queue_info_av_pkt = Get_queue_info_pointer(p.sys_state, recv_av_pkt_q->q_info.id);

    //prepare UTP socket.
    int welcome_udp_sd = 0;
    struct sockaddr_in udp_sa;
    bzero(&udp_sa,sizeof(struct sockaddr_in));
    int port = 0;
    port = recv_av_pkt_q->port;

    err = Start_udp_network_service(
              port,
              &welcome_udp_sd,
              &udp_sa,
              "UDP PORT");

    printf("-->Initialize the UDP (port %d) communication with clients :", port);
    if (-1 == err) {
        OUTPUT_ERROR;
        return ((void *) 1);
    } else {
        OUTPUT_OK;
    }

    char *buf_recv = NULL;
    buf_recv = (char*)malloc(MAX_SIZE_BUFFER_RECV_T);
    assert(NULL!=buf_recv);
    if (NULL==buf_recv) {
        OUTPUT_ERROR;
        return ((void *) 1);
    }

    int pkt_size = 0;
    T_Recv_Av_Packet_Node *p_node = NULL;
    T_Recv_Av_Packet_Node *p_tmp_node = NULL;

    //Dead loop.
    for (;;) {

        //monitor probe
        p_queue_info_av_pkt->length = recv_av_pkt_q->q_info.length;

        //receive the av packets from the Internet.
        //insert the packet into the queue of av packets.

        bzero(buf_recv, MAX_SIZE_BUFFER_RECV_T);

        //Monitor probe
        Set_worker_state(p_worker_info, WAITING_STATE);

        pkt_size = recvfrom(welcome_udp_sd, buf_recv, MAX_SIZE_BUFFER_RECV_T-1,
                            0, NULL, NULL);
//      DBG("%s(%d)%s(%d) :|%s|\n","-->Udp Port",port," recv av packet: ", pkt_size, buf_recv);
        DBG("%s(%d)%s(LEN %d,ID %d):||\n","-->Udp Port",port," Recv packet: ", pkt_size, pkt_id++);
        if (0>=pkt_size) {
            usleep(10);
            continue;
        }

        p_node = NULL;
        p_tmp_node = NULL;

        //Monitor probe
        //Set_worker_state(p_worker, TAKENODE_STATE);
        Set_worker_state(p_worker_info, WORKING_STATE);

        p_node = malloc(sizeof(T_Recv_Av_Packet_Node));
        assert(NULL!=p_node);
        bzero(p_node,sizeof(T_Recv_Av_Packet_Node));
		
        p_node->id = pkt_id;
        p_node->av_pkt = malloc(pkt_size);
        memcpy(p_node->av_pkt, buf_recv, pkt_size);

/*		free(p_node->av_pkt);
		p_node->av_pkt = NULL;
		free(p_node);
		p_node = NULL;
		continue;
*/
        //if the limit of queue length is reached, or else...
        if (-1==Check_recv_av_packet_queue_limit(recv_av_pkt_q)) {
            err = Remove_recv_av_packet_queue(recv_av_pkt_q, &p_tmp_node);
            assert(NULL!=p_tmp_node);
            free( p_tmp_node->av_pkt );
			p_tmp_node->av_pkt = NULL;
            free(p_tmp_node);
            p_tmp_node = NULL;
        }

        err = Insert_recv_av_packet_queue(recv_av_pkt_q, p_node);

        //Monitor probe
        Set_worker_state(p_worker_info, PLACENODE_STATE);
        Set_worker_state(p_worker_info, COMPLETED_STATE);
    }

    //just for the codes completeness.
    free(buf_recv);
    buf_recv = NULL;
    return ((void *) 0);
}

