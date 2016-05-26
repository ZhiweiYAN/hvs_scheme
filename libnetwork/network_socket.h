/*
 * =====================================================================================
 *
 *       Filename:  network_socket.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  4/7/2011 10:28:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  NETWORK_SOCKET_H_INC
#define  NETWORK_SOCKET_H_INC
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

#include "log_errors.h"

#define WELCOME_PORT 18000
#define NETWORK_BACK_LOG 200
#define MAX_SIZE_BUFFER_RECV 512*1024
#define MAX_SIZE_BUFFER_SEND 512*1024


int Start_tcp_network_service(int port,int *inpar_welcome_sd, struct sockaddr_in *inpar_sa, char* inpar_s);
int Start_udp_network_service(int port,int *inpar_welcome_sd, struct sockaddr_in *inpar_sa, char* inpar_s);


#endif   /* ----- #ifndef NETWORK_SOCKET_H_INC  ----- */


