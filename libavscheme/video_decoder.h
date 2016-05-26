/*
 * =====================================================================================
 *
 *       Filename:  video_decoder.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/15/2010 9:16:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  VIDEO_DECODER_H_INC
#define  VIDEO_DECODER_H_INC

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include "log_errors.h"
#include "network_socket.h"
#include "manage_queue.h"
#include "worker_manager.h"
#include "system_monitor.h"


typedef struct {
    T_Recv_Av_Packet_Queue *av_pkt_q;
    T_Decoded_Av_Frame_Queue *av_frm_q;
}T_AV_Packet_Frame_Queues;

typedef struct {
    int worker_id;
    T_Recv_Av_Packet_Queue *av_pkt_q;
    T_Decoded_Av_Frame_Queue *av_frm_q;
    T_System_State *sys_state;
}T_Decoder_Thread_Param;

void * Decode_av_packets(T_Decoder_Thread_Param *param);
int Initialize_decode_av_packet_pthreads(T_Recv_Av_Packet_Queue **av_pkt_q,
        T_Decoded_Av_Frame_Queue **av_frm_q,T_System_State *sys_state);

#endif   /* ----- #ifndef VIDEO_DECODER_H_INC  ----- */
