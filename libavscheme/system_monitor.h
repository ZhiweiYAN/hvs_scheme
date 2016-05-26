/*
 * =====================================================================================
 *
 *       Filename:  system_monitor.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/24/2010 4:10:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  SYSTEM_MONITOR_H_INC
#define  SYSTEM_MONITOR_H_INC

#include "network_socket.h"
#include "manage_queue.h"
#include "worker_manager.h"

#define  MONITOR_SERVER_PORT 19000
#define  MONITOR_CLIENT_PORT 19100
#define  SYSTEM_MONITOR_SHARE_ID 74
#define  QUEUE_MULTIPLE 2
#define  REST_QUEUE_NUM 2
#define  WORKER_MULTIPLE 2
#define  REST_WORKER_NUM 3

//Queue info
typedef struct {
    int client_num;
    int queue_num ;
    int rec_queue_num ;
    int dec_queue_num ;
    int mix_queue_num ;
    int enc_queue_num ;	

    int worker_num ;
    int rec_thread_num ;
    int dec_thread_num ;
    int mix_thread_num ;
    int enc_thread_num ;
    int snd_thread_num ;
    T_Queue_Info *qs_info;
    T_Worker_Info *ws_info;
}T_System_State;

typedef struct {
    T_System_State *sys_state;
}T_Monitor_Thread_Param;

typedef struct {
    T_System_State ss;
}T_Monitor_Packet;

int Initialize_system_state_memory(T_System_State **sys_state, int client_num);
int Destory_system_state_memory(T_System_State *sys_state);

int Initialize_system_state_monitor(T_System_State *sys_state, int client_num);

void * Dameon_monitor(T_Monitor_Thread_Param *param);
T_Worker_Info *Get_worker_info_pointer(T_System_State *sys_state, int worker_id);
T_Queue_Info *Get_queue_info_pointer(T_System_State *sys_state, int queue_id);


#endif   /* ----- #ifndef SYSTEM_MONITOR_H_INC  ----- */
