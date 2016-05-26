/*
 * =====================================================================================
 *
 *       Filename:  recv_av_packet.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/13/2010 8:55:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  RECV_AV_PACKET_H_INC
#define  RECV_AV_PACKET_H_INC

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <signal.h>
#include <pthread.h>

#include "log_errors.h"
#include "network_socket.h"
#include "manage_queue.h"
#include "system_monitor.h"
#include "worker_manager.h"

typedef struct {
    int worker_id;
    T_Recv_Av_Packet_Queue *q;
    T_System_State *sys_state;
}T_Recv_Thread_Param;

int Initialize_recv_av_packet_pthreads(T_Recv_Av_Packet_Queue **qs, T_System_State *sys_state);
void * Recv_av_packets(T_Recv_Thread_Param *param);
#endif   /* ----- #ifndef RECV_AV_PACKET_H_INC  ----- */
