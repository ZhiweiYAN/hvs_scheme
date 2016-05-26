/*
 * =====================================================================================
 *
 *       Filename:  hvs_main.c
 *
 *    Description:  Main function of the hybrid video mixer
 *
 *        Version:  1.0
 *        Created:  2011-04-07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
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

#include "log_errors.h"
#include "network_socket.h"
#include "hvs_init.h"

int main (void)
{

    //run log errors
    Log_begin();

    //run video mixer manager which receives the
    //instructions from the FOCUS server
    int err = 0;
    int welcome_sd_focus = 0;
    struct sockaddr_in sa_focus;

    bzero(&sa_focus,sizeof(struct sockaddr_in));
    err =Start_tcp_network_service(
             WELCOME_PORT,
             &welcome_sd_focus,
             &sa_focus,
             "Mixer Server");
    printf("Initialize the communication with FOCUS server :");
    if (-1 == err) {
        OUTPUT_ERROR;
        return -1;
    } else {
        OUTPUT_OK;
    }
    //run a dameon to accept the request from the Focus server
    err = Create_hybrid_video_process(welcome_sd_focus,&sa_focus);
    if (-1 == err) {
        OUTPUT_ERROR;
        return -1;
    } else {
        OUTPUT_OK;
    }
}
