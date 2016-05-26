/*
 * =====================================================================================
 *
 *       Filename:  worker_manager.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/28/2010 7:02:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "worker_manager.h"

inline int Set_worker_state(T_Worker_Info *wi, T_Enum_Worker_State s)
{
    assert(NULL!=wi);
    wi->state = s;
    //DBG("%s %d\n","Set worker_state as", (int) s);
    return 1;
}

int Set_worker_type(T_Worker_Info *wi, T_Enum_Worker_Type t)
{
    assert(NULL!=wi);
    wi->type = t;
    return 1;
}

int Set_worker_id( T_Worker_Info *wi, int id)
{
    assert( NULL!=wi );
    wi->id = id;
    return 1;
}


