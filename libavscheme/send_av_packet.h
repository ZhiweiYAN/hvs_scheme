/*
 * =====================================================================================
 *
 *       Filename:  send_av_packet.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/21/2010 10:00:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  SEND_AV_PACKET_H_INC
#define  SEND_AV_PACKET_H_INC

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
#include "worker_manager.h"
#include "system_monitor.h"

typedef struct {
    int worker_id;
    T_Encoded_Av_Packet_Queue *encoded_av_pkt_q;
    T_System_State *sys_state;
}T_Send_Thread_Param;

int Initialize_send_av_packet_pthread(T_Encoded_Av_Packet_Queue *q, T_System_State *sys_state);
void *Send_av_packets(T_Send_Thread_Param *param);
#endif   /* ----- #ifndef SEND_AV_PACKET_H_INC  ----- */
