/*
 * =====================================================================================
 *
 *       Filename:  system_monitor.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/24/2010 4:11:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "system_monitor.h"
#define BUFFER_SIZE 2048

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_system_state
 *  Description:
 * =====================================================================================
 */
int Initialize_system_state_monitor(T_System_State *sys_state, int client_num)
{
    int err = 0;

    T_Monitor_Thread_Param *param = NULL;
    param = malloc( sizeof(T_Monitor_Thread_Param) );
    assert(NULL!=param);

    param->sys_state = sys_state;

    pthread_t tid = 0;

    err = pthread_create(&tid, NULL, (void *)Dameon_monitor,  (void *) param);

    if (0 != err) {
        OUTPUT_ERROR;
        return -1;
    }

    return 1;
}


void * Dameon_monitor(T_Monitor_Thread_Param *param)
{

    assert(NULL!=param);
    int err = 0;

    T_Monitor_Thread_Param p;
    memcpy(&p, param, sizeof(T_Monitor_Thread_Param) );

    free(param);
    param = NULL;

    T_Queue_Info *p_qs = NULL;
    T_Worker_Info *p_ws = NULL;
    p_qs = p.sys_state->qs_info;
    p_ws = p.sys_state->ws_info;

    int client_num = 0;
    int queue_num = 0;
    int worker_num = 0;
    client_num = p.sys_state->client_num;
    queue_num = p.sys_state->queue_num;
    worker_num = p.sys_state->worker_num;


    //prepare UTP socket.
    int welcome_udp_sd = 0;
    struct sockaddr_in s_addr;
    struct sockaddr_in c_addr;
    socklen_t addr_len;
    bzero(&s_addr,sizeof(struct sockaddr_in));
    bzero(&c_addr,sizeof(struct sockaddr_in));

    err = Start_udp_network_service(
              MONITOR_SERVER_PORT,
              &welcome_udp_sd,
              &s_addr,
              "Monitor UDP PORT");

    printf("-->Initialize the Monitor UDP (port %d) communication with clients :", MONITOR_SERVER_PORT);
    if (-1 == err) {
        OUTPUT_ERROR;
        return ((void *) 1);
    } else {
        OUTPUT_OK;
    }

    char *buf = NULL;
    char *q = NULL;
    int len = 0;
    //Dead loop.
    for (;;) {

        //receive the av packets from the Internet.
        //insert the packet into the queue of av packets.
        buf = (char*)malloc(BUFFER_SIZE);
        assert(NULL!=buf);
        bzero(buf, BUFFER_SIZE);

        addr_len = sizeof(struct sockaddr_in);
        len = recvfrom(welcome_udp_sd, buf, BUFFER_SIZE-1,
                       0, (struct sockaddr *) &c_addr, &addr_len);
        DBGwTIME("%s(%d)%s|%s|_%s","-->Udp Port",MONITOR_SERVER_PORT," recv monitor pkt:", buf);
        if (len<0) {
            usleep(10);
            free(buf);
            buf = NULL;
            continue;
        }

        //Send system state packet.
        bzero(buf, BUFFER_SIZE);
        len = 0;
        q = buf;

        memcpy(q, p.sys_state, sizeof(T_System_State) );
        q += sizeof(T_System_State);
        len += sizeof(T_System_State);

        //Packet part: queue state
        memcpy(q, p_qs, queue_num * sizeof(T_Queue_Info) );
        q += queue_num * sizeof(T_Queue_Info);
        len += queue_num * sizeof(T_Queue_Info);

        //Packet part: threads state
        memcpy(q, p_ws, worker_num * sizeof(T_Worker_Info) );
        len += worker_num * sizeof(T_Worker_Info);

        //Sending packet.
        len  = sendto(welcome_udp_sd, buf, len,0,
                      (struct sockaddr *) &c_addr, addr_len);
        free(buf);
        buf = NULL;
    }//for loop end

    //just for codes completeness
    return (void*) 0;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_system_state_memory
 *  Description:
 * =====================================================================================
 */
int Initialize_system_state_memory(T_System_State **sys_state, int client_num)
{
    assert(NULL!=sys_state);
    assert(1<=client_num);

    //allocate the memory of system state
    (*sys_state) = malloc( sizeof(T_System_State) );
    assert(NULL!=(*sys_state));
    bzero( (*sys_state), sizeof(T_System_State) );

    T_System_State *s = NULL;
    s = *sys_state;

    s->client_num = client_num;

    int queue_num = 0;
    int rec_queue_num = 0;
    int dec_queue_num = 0;
    int mix_queue_num = 0;
    int enc_queue_num = 0;

    rec_queue_num = client_num;
    dec_queue_num = client_num;
    mix_queue_num = 1;
    enc_queue_num = 1;

    queue_num = rec_queue_num + dec_queue_num + mix_queue_num + enc_queue_num;

    s->queue_num = queue_num;
    s->rec_queue_num = rec_queue_num;
    s->dec_queue_num = dec_queue_num;
    s->mix_queue_num = mix_queue_num;
    s->enc_queue_num = enc_queue_num;

    int worker_num = 0;
    int rec_thread_num = 0;
    int dec_thread_num = 0;
    int mix_thread_num = 0;
    int enc_thread_num = 0;
    int snd_thread_num = 0;

    rec_thread_num = client_num;
    dec_thread_num = client_num;
    mix_thread_num = client_num - 1;
    enc_thread_num = 1;
    snd_thread_num = 1;
    worker_num = rec_thread_num + dec_thread_num
                 + mix_thread_num + enc_thread_num + snd_thread_num;

    s->worker_num = worker_num;
    s->rec_thread_num = rec_thread_num;
    s->dec_thread_num = dec_thread_num;
    s->mix_thread_num = mix_thread_num;
    s->enc_thread_num = enc_thread_num;
    s->snd_thread_num = snd_thread_num;

    s->qs_info = malloc( queue_num * sizeof(T_Queue_Info) );
    assert(NULL!=s->qs_info);
    bzero( s->qs_info, queue_num * sizeof(T_Queue_Info) );
    T_Queue_Info *p = NULL;
    p = s->qs_info;
    for (int i=0; i<queue_num; i++) {
        p->id = i;
        p = p + 1;
    }

    s->ws_info = malloc( worker_num * sizeof(T_Worker_Info) );
    assert(NULL!=s->ws_info);
    bzero( s->ws_info, worker_num * sizeof(T_Worker_Info) );
    T_Worker_Info *q = NULL;
    q = s->ws_info;
    for (int i=0; i<worker_num; i++) {
        q->id = i;
        q = q + 1;
    }

    return 1;
}

int Destory_system_state_memory(T_System_State *sys_state)
{
    assert(NULL!=sys_state);

    free(sys_state->qs_info);
    free(sys_state->ws_info);
    sys_state->qs_info = NULL;
    sys_state->ws_info = NULL;

    free(sys_state);
    sys_state = NULL;

    return 1;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  T_Worker_Info *Get_worker_info_pointer(T_System_State *sys_state, int worker_id)
 *  Description:
 * =====================================================================================
 */
T_Worker_Info *Get_worker_info_pointer(T_System_State *sys_state, int worker_id)
{
    T_Worker_Info *p_worker_info = NULL;

    p_worker_info = sys_state->ws_info;
    p_worker_info = p_worker_info + worker_id;

    assert(p_worker_info->id == worker_id);

    return p_worker_info;

}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Get_queue_info_pointer(T_System_State *, int)
 *  Description:
 * =====================================================================================
 */

T_Queue_Info *Get_queue_info_pointer(T_System_State *sys_state, int queue_id)
{
    T_Queue_Info *p_queue_info = NULL;

    p_queue_info = sys_state->qs_info;
    p_queue_info = p_queue_info + queue_id;

    assert(p_queue_info->id == queue_id );

    return p_queue_info;
}
