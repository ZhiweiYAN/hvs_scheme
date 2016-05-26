/*
 * =====================================================================================
 *
 *       Filename:  video_encoder.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/19/2010 1:33:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  VIDEO_ENCODER_H_INC
#define  VIDEO_ENCODER_H_INC

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "log_errors.h"
#include "manage_queue.h"
#include "worker_manager.h"
#include "system_monitor.h"

typedef struct {
    T_Mixed_Av_Frame_Queue *mixed_av_frm_q;
    T_Encoded_Av_Packet_Queue *encoded_av_pkt_q;
}T_Mixed_Encoded_Queues_info;

typedef struct {
    int worker_id;
    T_Mixed_Av_Frame_Queue *mixed_av_frm_q;
    T_Encoded_Av_Packet_Queue *encoded_av_pkt_q;
    T_System_State *sys_state;
}T_Encoder_Thread_Param;

int Initialize_video_encoder_pthread( T_Mixed_Av_Frame_Queue *mixed_av_frm_q,
                                      T_Encoded_Av_Packet_Queue *encoded_av_pkt_q, T_System_State *sys_state);

void * Encode_Av_Frames(T_Encoder_Thread_Param *param);

#endif   /* ----- #ifndef VIDEO_ENCODER_H_INC  ----- */
