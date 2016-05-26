/*
 * =====================================================================================
 *
 *       Filename:  worker_manager.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/28/2010 7:02:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  WORKER_MANAGER_H_INC
#define  WORKER_MANAGER_H_INC

#include <assert.h>
#include <stdio.h>
#include "log_errors.h"

//Worker threads
typedef enum {
    WAITING_STATE = 0,
    TAKENODE_STATE,
    WORKING_STATE,
    PLACENODE_STATE,
    COMPLETED_STATE,
}T_Enum_Worker_State;

typedef enum {
    UNKNOWN_WORKER = 0,
    RECEIVER_WORKER,
    DECODEDER_WORKER,
    MIXER_WORKER,
    ENCODER_WORKER,
    SENDER_WORKER,
}T_Enum_Worker_Type;

typedef struct {
    int id;
    T_Enum_Worker_Type type;
    T_Enum_Worker_State state;
}T_Worker_Info;


int Set_worker_state(T_Worker_Info *wi, T_Enum_Worker_State s);
int Set_worker_type(T_Worker_Info *wi, T_Enum_Worker_Type t);
int Set_worker_id( T_Worker_Info *wi, int id);



#endif   /* ----- #ifndef WORKER_MANAGER_H_INC  ----- */
