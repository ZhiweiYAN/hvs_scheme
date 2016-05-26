/*
 * =====================================================================================
 *
 *       Filename:  network_socket.c
 *
 *    Description:  Handle network communications
 *
 *        Version:  1.0
 *        Created:  4/7/2011 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#include "network_socket.h"


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Start_tcp_network_service
 *  Description:  Open TCP ports for the network connections as a server such as binding port and
 *  			  listening port.
 * =====================================================================================
 */
int Start_tcp_network_service(int inpar_port,
                              int *inpar_welcome_sd,
                              struct sockaddr_in *inpar_sa,
                              char* inpar_msg)
{
    assert(NULL!=inpar_welcome_sd);
    assert(NULL!=inpar_sa);

    int reuse =1;
    int retry_counter = 0;

    /* Check input parameters */
    if (1024>inpar_port||NULL==inpar_welcome_sd||NULL==inpar_sa) {
        OUTPUT_ERROR;
        return -1;
    }

    /* Create a socket */
    if ((*inpar_welcome_sd=socket(AF_INET,SOCK_STREAM,0))<0) {
        OUTPUT_ERROR;
        return -1;
    }

    /* Bind the socket to the inpar_port, and allow all IP connect with it */
    bzero(inpar_sa,sizeof(struct sockaddr_in));
    inpar_sa->sin_family = AF_INET;
    inpar_sa->sin_port = htons(inpar_port);
    inpar_sa->sin_addr.s_addr = htonl(INADDR_ANY);

    /* Elimitate the message "Address already in use" */
    if (setsockopt(*inpar_welcome_sd,SOL_SOCKET,SO_REUSEADDR, &reuse, sizeof(int))<0) {
        OUTPUT_ERROR;
    }
    do {
        if (0==bind(*inpar_welcome_sd,(struct sockaddr *)inpar_sa,sizeof(struct sockaddr_in))) {
            /* success for bind operation */
            printf("%s opens(binds) %d port. ", inpar_msg, inpar_port);
            OUTPUT_OK;
            break;
        } else {
            printf("%s opens(binds) %d port. ", inpar_msg, inpar_port);
            OUTPUT_ERROR;
            printf("The %s does the %d-th trial to bind %d port after 5 sec.\n",inpar_msg, ++retry_counter, inpar_port);
            sleep(5);
            continue;
        }
    } while (1);

    listen(*inpar_welcome_sd, NETWORK_BACK_LOG);

    return 1;

}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Start_udp_network_service
 *  Description:  Open UDP ports for the network connections as a server such as binding port and
 *  			  listening port.
 * =====================================================================================
 */
int Start_udp_network_service(int inpar_port,
                              int *inpar_welcome_sd,
                              struct sockaddr_in *inpar_sa,
                              char* inpar_msg)
{
    assert(NULL!=inpar_welcome_sd);
    assert(NULL!=inpar_sa);

    int reuse =1;
    int retry_counter = 0;

    /* Check input parameters */
    if (1024>inpar_port||NULL==inpar_welcome_sd||NULL==inpar_sa) {
        OUTPUT_ERROR;
        return -1;
    }

    /* Create a socket */
    if ((*inpar_welcome_sd=socket(AF_INET,SOCK_DGRAM,0))<0) {
        OUTPUT_ERROR;
        return -1;
    }

    /* Bind the socket to the inpar_port, and allow all IP connect with it */
    bzero(inpar_sa,sizeof(struct sockaddr_in));
    inpar_sa->sin_family = AF_INET;
    inpar_sa->sin_port = htons(inpar_port);
    inpar_sa->sin_addr.s_addr = htonl(INADDR_ANY);

    /* Elimitate the message "Address already in use" */
    if (setsockopt(*inpar_welcome_sd,SOL_SOCKET,SO_REUSEADDR, &reuse, sizeof(int))<0) {
        OUTPUT_ERROR;
    }
    do {
        if (0==bind(*inpar_welcome_sd,(struct sockaddr *)inpar_sa,sizeof(struct sockaddr_in))) {
            /* success for bind operation */
            printf("%s opens(binds) %d port. ", inpar_msg, inpar_port);
            OUTPUT_OK;
            break;
        } else {
            printf("%s opens(binds) %d port. ", inpar_msg, inpar_port);
            OUTPUT_ERROR;
            printf("The %s does the %d-th trial to bind %d port after 5 sec.\n",inpar_msg, ++retry_counter, inpar_port);
            sleep(5);
            continue;
        }
    } while (1);

    listen(*inpar_welcome_sd, NETWORK_BACK_LOG);

    return 1;

}

