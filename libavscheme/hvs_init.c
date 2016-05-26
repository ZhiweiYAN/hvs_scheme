/*
 * =====================================================================================
 *
 *       Filename:  hvs_init.c
 *
 *    Description:
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
#include "log_errors.h"
#include "manage_queue.h"
#include "network_socket.h"
#include "recv_av_packet.h"
#include "video_decoder.h"
#include "video_mixer.h"
#include "video_encoder.h"
#include "send_av_packet.h"
#include "system_monitor.h"
/* for example, 4 clients
+------------+     +------------+      +------------+      +------------+
| RTP RECV1  |-----| RTP Queu1  |------|  decoder   |------| YUV Queu1  |\
+------------+     +------------+      +------------+      +------------+ \
+------------+     +------------+      +------------+      +------------+  \
| RTP RECV2  |-----| RTP Queu2  |------|  decoder   |------| YUV Queu|  |   \
+------------+     +------------+      +------------+      +------------+    \
+------------+     +------------+      +------------+      +------------+     X----+
| RTP RECV3  |-----| RTP Queu3  |------|  decoder   |------| YUV Queu|  |    /     |
+------------+     +------------+      +------------+      +------------+   /      |
+------------+     +------------+      +------------+      +------------+  /       |
| RTP RECV4  |-----| RTP Queu4  |---+--|  decoder   |------| YUV Queu4  | /        |
+------------+     +------------+      +------------+      +------------+          |
                                                                                   |
   +-------------------------------------------------------------------------------+
   |
   |
   |     +--------------+   +-----------------+     +------------+
   +---> | YUV Mixer    |---| Mixer YUV Queue |-----| encoder    |-----------------+
         +--+-------+--++   +-----------------+     +--+-------+-+                 |
                                                                                   |
   +-------------------------------------------------------------------------------+
   |
   |                            +------------+
   |                            | RTP send1  |
   |                            +------------+
   |     +-----------------+    +------------+
   |     |                 |    | RTP send2  |
   +-----+ Video Pkt Queu  +----+------------+-
         |                 |    +------------+
         +-----------------+    | RTP send3  |
                                +------------+
                                +------------+
                                | RTP send4  |
                                +------------+
*/
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Create_hybrid_video_process()
 *  Description:  A new connection from outside will triger the process of video mixer,
 *                which initializes the queue of RECV, the queue of DECODED, the queue
 *                of MIXED YUVs, the queue of ENCODED PACKETS and worker processes of
 *                RECEIVER, DECODER, MIXER, ENCODER, and SENDER.
 * =====================================================================================
 */

#define C_BUFFER_SIZE 1000
#include <stdio.h>
#include <stdlib.h>
int Create_hybrid_video_process_x(int inpar_welcome_sd,struct sockaddr_in *inpar_sa)
{
    int err = 0;
    pid_t pid = 0;

    int connection_sd = 0;
    socklen_t len = 0;
    int count = 0;
    char *buf_recv = NULL;

    assert(0<inpar_welcome_sd);
    assert(NULL!=inpar_sa);

    len = sizeof (struct sockaddr);

//    av_register_all();

    /* Enter the Daemon */
    while (1) {
        printf("The Pid (%d) Ready for the admission of one conference.\n", getpid());
        if (( connection_sd = accept(inpar_welcome_sd,(struct sockaddr*)inpar_sa,&len))<0) {
            OUTPUT_ERROR;
            close(connection_sd);
            sleep(1);
            continue;
        }

        printf("---> A connetion has been accepted and a process will be created.\n");

//  create a seperate process to monitor the process table.
        if ((pid = fork()) < 0) {
            OUTPUT_ERROR;
            close(connection_sd);
            return -1;
        } else if (0 == pid) {

//      enter into the child process, child name=alice, mother=obama
            /* Close the listening socket description */

            close(inpar_welcome_sd);

            if ((pid = fork()) < 0) {
                OUTPUT_ERROR;
                return -1;
            } else if (pid > 0)
                exit(0); /* the child "alice" return to her mother "obama" */


//          enter into the grandchild, grandchild name=jose, mother=alice
            DBGwTIME("The hybrid video mixer process(%d) is running now.%s", getpid());

            /* In the grandchild process */
            /* Allocate memory for receiving data */
            buf_recv =  (char*)malloc(C_BUFFER_SIZE);
            assert(NULL!=buf_recv);
            bzero(buf_recv, C_BUFFER_SIZE);

            //receive the instructions from a focus server.
            count = recv(connection_sd,buf_recv,C_BUFFER_SIZE-1,0);
            DBG("\n%s:|%s|\n","Recv data",buf_recv);
            count = send(connection_sd,buf_recv,count,0);
            DBG("\n%s:|%s|\n","Send data",buf_recv);
            close(connection_sd);

            //parse the instruction from the focus manager
            if (0 == strncmp("x",buf_recv,1) ) {
                exit(0);
            }

            free(buf_recv);
            buf_recv = NULL;

            //handle the data in the recv buffer.
            //....
            //....
            int client_num= 4;
            int conference_duration = 13600;

            //Initilization Phase
            printf("%s", "\n\n******   System State Initilization Begin.   ******\n");
            printf("%s","Initialize the memory of System State Monitor.");
            T_System_State *sys_state = NULL;
            if (1 == Initialize_system_state_memory(&sys_state, client_num)) {
                OUTPUT_OK;
            } else {
                OUTPUT_ERROR;
            };
            printf("%s","Initialize the memory of System State Monitor.");
            err = Initialize_system_state_monitor(sys_state, client_num);

            printf("%s", "******   System Monitor Initilization End.   ******\n\n");

            //Data structures of four kinds of queues
            T_Recv_Av_Packet_Queue **recv_av_packet_queues = NULL;
            T_Decoded_Av_Frame_Queue **decoded_av_frame_queues = NULL;
            T_Mixed_Av_Frame_Queue *mixed_av_frame_queue = NULL;
            T_Encoded_Av_Packet_Queue *encoded_av_packet_queue = NULL;

            //Queues Initilization
            printf("%s", "\n\n******   Queues Initilization Begin.   ******\n");
            printf("%s", "Initialize the queues of Recv AV packets.(1/4)");
            if (1==Initialize_recv_av_packet_queues(&recv_av_packet_queues,
                                                    0,
                                                    sys_state->rec_queue_num)) {
                OUTPUT_OK;
            } else {
                OUTPUT_ERROR;
            };

            printf("%s","Initialize the queues of Decoded AV frames.(2/4)");
            if (1==Initialize_decoded_av_frame_queues(&decoded_av_frame_queues,
                    sys_state->rec_queue_num,
                    sys_state->dec_queue_num)) {
                OUTPUT_OK;
            } else {
                OUTPUT_ERROR;
            };

            printf("%s","Initialize the queues of Mixed  AV frames.(3/4)");
            if (1==Initialize_mixed_av_frame_queue(&mixed_av_frame_queue,
                                                   sys_state->rec_queue_num + sys_state->dec_queue_num,
                                                   sys_state->mix_queue_num)) {
                OUTPUT_OK;
            } else {
                OUTPUT_ERROR;
            };

            printf("%s","Initialize the queues of Encoded  AV packets.(4/4)");
            if (1==Initialize_encoded_av_packet_queue(&encoded_av_packet_queue,
                    sys_state->rec_queue_num + sys_state->dec_queue_num+sys_state->mix_queue_num,
                    sys_state->enc_queue_num)) {
                OUTPUT_OK;
            } else {
                OUTPUT_ERROR;
            };
            printf("%s", "******   Queues Initilization End.   ******\n");

            //Actions or five worker threads, which include Recv, Decoder, Mixer, Encoder, Sender
            printf("%s", "\n******   Threads Initilization Begin.   ******\n");
            if (1==Initialize_recv_av_packet_pthreads(
                    recv_av_packet_queues, sys_state)) {
                printf("%s","Initialize the pthreads of Recv AV packets.(1/5)");
                OUTPUT_OK;
            } else {
                printf("%s","Initialize the pthreads of Recv AV packets.(1/5)");
                OUTPUT_ERROR;
            };

            if (1== Initialize_decode_av_packet_pthreads(
                    recv_av_packet_queues,decoded_av_frame_queues,sys_state)) {
                printf("%s","Initialize the pthreads of Decoding AV packets.(2/5)");
                OUTPUT_OK;
            } else {
                printf("%s","Initialize the pthreads of Decoding AV packets.(2/5)");
                OUTPUT_ERROR;
            };

            if (1== Initialize_video_mixer_pthread(
                    decoded_av_frame_queues, mixed_av_frame_queue, sys_state)) {
                printf("%s","Initialize the pthreads of Video Mixer.(3/5)");
                OUTPUT_OK;
            } else {
                printf("%s","Initialize the pthreads of Video Mixer.(3/5)");
                OUTPUT_ERROR;
            };

            if (1== Initialize_video_encoder_pthread(
                    mixed_av_frame_queue,encoded_av_packet_queue, sys_state)) {
                printf("%s","Initialize the pthreads of Video Encoder.(4/5)");
                OUTPUT_OK;
            } else {
                printf("%s","Initialize the pthreads of Video Encoder.(4/5)");
                OUTPUT_ERROR;
            };

            if (1== Initialize_send_av_packet_pthread(
                    encoded_av_packet_queue, sys_state)) {
                printf("%s","Initialize the pthreads of Video Packet Sender.(5/5)");
                OUTPUT_OK;
            } else {
                printf("%s","Initialize the pthreads of Video Packet Sender.(5/5)");
                OUTPUT_ERROR;
            };
            printf("%s", "******   Threads Initilization End.   ******\n\n");


            //..

            sleep(conference_duration);

            DBGwTIME("\033[31mThe whole video hybrid conference process(%d) is exiting now\033[0m.%s", getpid());

            exit(0);

            return err;
        }

        /* Close the connection socket description in the parent obama  process*/
        close(connection_sd);

        /* In the parent process */
        if (waitpid(pid,NULL,0)!=pid) {
            OUTPUT_ERROR;
        }
        continue;
    }
    return 1;
}


int Create_hybrid_video_process(int inpar_welcome_sd,struct sockaddr_in *inpar_sa)
{
    int err = 0;

//    av_register_all();

//  
    DBGwTIME("The hybrid video mixer process(%d) is running now.%s", getpid());

    /* In the grandchild process */
    //handle the data in the recv buffer.
    //....
    //....
    int client_num= 4;
    int conference_duration = 13600;

    //Initilization Phase
    printf("%s", "\n\n******   System State Initilization Begin.   ******\n");
    printf("%s","Initialize the memory of System State Monitor.");
    T_System_State *sys_state = NULL;
    if (1 == Initialize_system_state_memory(&sys_state, client_num)) {
        OUTPUT_OK;
    } else {
        OUTPUT_ERROR;
    };
    printf("%s","Initialize the thread of System State Monitor.");
    err = Initialize_system_state_monitor(sys_state, client_num);

    printf("%s", "\n******   System Monitor Initilization End.   ******\n\n");

    //Data structures of four kinds of queues
    T_Recv_Av_Packet_Queue **recv_av_packet_queues = NULL;
    T_Decoded_Av_Frame_Queue **decoded_av_frame_queues = NULL;
    T_Mixed_Av_Frame_Queue *mixed_av_frame_queue = NULL;
    T_Encoded_Av_Packet_Queue *encoded_av_packet_queue = NULL;

    //Queues Initilization
    printf("%s", "\n\n******   Queues Initilization Begin.   ******\n");
    printf("%s", "Initialize the queues of Recv AV packets.(1/4)");
    if (1==Initialize_recv_av_packet_queues(&recv_av_packet_queues,
                                            0,
                                            sys_state->rec_queue_num)) {
        OUTPUT_OK;
    } else {
        OUTPUT_ERROR;
    };

    printf("%s","Initialize the queues of Decoded AV frames.(2/4)");
    if (1==Initialize_decoded_av_frame_queues(&decoded_av_frame_queues,
            sys_state->rec_queue_num,
            sys_state->dec_queue_num)) {
        OUTPUT_OK;
    } else {
        OUTPUT_ERROR;
    };

    printf("%s","Initialize the queues of Mixed  AV frames.(3/4)");
    if (1==Initialize_mixed_av_frame_queue(&mixed_av_frame_queue,
                                           sys_state->rec_queue_num + sys_state->dec_queue_num,
                                           sys_state->mix_queue_num)) {
        OUTPUT_OK;
    } else {
        OUTPUT_ERROR;
    };

    printf("%s","Initialize the queues of Encoded  AV packets.(4/4)");
    if (1==Initialize_encoded_av_packet_queue(&encoded_av_packet_queue,
            sys_state->rec_queue_num + sys_state->dec_queue_num+sys_state->mix_queue_num,
            sys_state->enc_queue_num)) {
        OUTPUT_OK;
    } else {
        OUTPUT_ERROR;
    };
    printf("%s", "******   Queues Initilization End.   ******\n");

    //Actions or five worker threads, which include Recv, Decoder, Mixer, Encoder, Sender
    printf("%s", "\n******   Threads Initilization Begin.   ******\n");
    if (1==Initialize_recv_av_packet_pthreads(
            recv_av_packet_queues, sys_state)) {
        printf("%s","Initialize the pthreads of Recv AV packets.(1/5)");
        OUTPUT_OK;
    } else {
        printf("%s","Initialize the pthreads of Recv AV packets.(1/5)");
        OUTPUT_ERROR;
    };

    if (1== Initialize_decode_av_packet_pthreads(
            recv_av_packet_queues,decoded_av_frame_queues,sys_state)) {
        printf("%s","Initialize the pthreads of Decoding AV packets.(2/5)");
        OUTPUT_OK;
    } else {
        printf("%s","Initialize the pthreads of Decoding AV packets.(2/5)");
        OUTPUT_ERROR;
    };

    if (1== Initialize_video_mixer_pthread(
            decoded_av_frame_queues, mixed_av_frame_queue, sys_state)) {
        printf("%s","Initialize the pthreads of Video Mixer.(3/5)");
        OUTPUT_OK;
    } else {
        printf("%s","Initialize the pthreads of Video Mixer.(3/5)");
        OUTPUT_ERROR;
    };

    if (1== Initialize_video_encoder_pthread(
            mixed_av_frame_queue,encoded_av_packet_queue, sys_state)) {
        printf("%s","Initialize the pthreads of Video Encoder.(4/5)");
        OUTPUT_OK;
    } else {
        printf("%s","Initialize the pthreads of Video Encoder.(4/5)");
        OUTPUT_ERROR;
    };

    if (1== Initialize_send_av_packet_pthread(
            encoded_av_packet_queue, sys_state)) {
        printf("%s","Initialize the pthreads of Video Packet Sender.(5/5)");
        OUTPUT_OK;
    } else {
        printf("%s","Initialize the pthreads of Video Packet Sender.(5/5)");
        OUTPUT_ERROR;
    };
    printf("%s", "******   Threads Initilization End.   ******\n\n");


    //..

    sleep(conference_duration);

    DBGwTIME("\033[31mThe whole video hybrid conference process(%d) is exiting now\033[0m.%s", getpid());

	//free memory
	err = Destory_system_state_memory(sys_state);

    return err;
}

