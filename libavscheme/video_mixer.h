/*
 * =====================================================================================
 *
 *       Filename:  video_mixer.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/16/2010 9:26:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  VIDEO_MIXER_H_INC
#define  VIDEO_MIXER_H_INC
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
    T_Decoded_Av_Frame_Queue **decoded_av_frm_qs;
    T_Mixed_Av_Frame_Queue *mixed_av_frm_qs;
    int frm_n;
}T_Decoded_Mixed_Queues_Info;

typedef struct {
    int worker_id;
    T_Decoded_Av_Frame_Queue **decoded_av_frm_qs;
    T_Mixed_Av_Frame_Queue *mixed_av_frm_qs;
    T_System_State *sys_state;
}T_Mixer_Thread_Param;

int Initialize_video_mixer_pthread(T_Decoded_Av_Frame_Queue **decoded_av_frm_q,
                                   T_Mixed_Av_Frame_Queue *mixed_av_frm_q,  T_System_State *sys_state);
void * Mix_av_frames(T_Mixer_Thread_Param *param);

#endif   /* ----- #ifndef VIDEO_MIXER_H_INC  ----- */
