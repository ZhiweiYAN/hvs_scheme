/*
 * =====================================================================================
 *
 *       Filename:  manage_queue.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/9/2010 5:51:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  MANANGE_QUEUE_H_INC
#define  MANANGE_QUEUE_H_INC

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
#include <pthread.h>

#include "log_errors.h"

#define RECV_AV_QUEUE_PORT 18000

#define RECV_QUEUE_IDX_BASE 	0
#define DECODED_QUEUE_IDX_BASE 	20
#define MIXED_QUEUE_IDX_BASE 	30
#define ENCODED_QUEUE_IDX_BASE 	40

#define  MAX_AV_FRAME_NUM  		150
#define  MAX_AV_PACKET_NUM 		200

typedef enum {
    UNKNOWN_QUEUE = 0,
    RECEIVE_QUEUE,
    DECODED_QUEUE,
    MIXED_QUEUE,
    ENCODED_QUEUE,
}T_Enum_Queue_Type;

typedef struct {
    T_Enum_Queue_Type type;
    int id;
    int length;
}T_Queue_Info;


//AV packet queue definition, RECEIVING
typedef struct {
    int id;
    void *av_pkt;
    struct T_Recv_Av_Packet_Node *next;
}T_Recv_Av_Packet_Node;

typedef struct {
    T_Recv_Av_Packet_Node *header;
    T_Recv_Av_Packet_Node *tail;
    T_Queue_Info q_info;
    int port;
    pthread_mutex_t mutex;
}T_Recv_Av_Packet_Queue;

//decoded YUV frame queue definition
typedef struct {
    int id;
    void *av_frame;
    struct T_Decoded_Av_Frame_Node *next;
}T_Decoded_Av_Frame_Node;

typedef struct {
    T_Decoded_Av_Frame_Node *header;
    T_Decoded_Av_Frame_Node *tail;
    T_Queue_Info q_info;
    pthread_mutex_t mutex;
}T_Decoded_Av_Frame_Queue;

//mixed YUV frame queue definition
typedef struct {
    int id;
    void *av_frame;
    struct T_Mixed_Av_Frame_Node *next;
}T_Mixed_Av_Frame_Node;

typedef struct {
    T_Mixed_Av_Frame_Node *header;
    T_Mixed_Av_Frame_Node *tail;
    T_Queue_Info q_info;
    pthread_mutex_t mutex;
}T_Mixed_Av_Frame_Queue;

//encoded packet queue definition
typedef struct {
    int id;
    void *av_pkt;
    struct T_Encoded_Av_Packet_Node *next;
}T_Encoded_Av_Packet_Node;

typedef struct {
    T_Encoded_Av_Packet_Node *header;
    T_Encoded_Av_Packet_Node *tail;
    T_Queue_Info q_info;
    pthread_mutex_t mutex;
}T_Encoded_Av_Packet_Queue;

//AV packet queue definition, SENDING, Maybe RTP format.
/*
typedef struct {
    int id;
    AVPacket av_pkt;
    struct T_Send_Av_Packet_Node *next;
}T_Send_Av_Packet_Node;

typedef struct {
    int id;
    T_Send_Av_Packet_Node *header;
    T_Send_Av_Packet_Node *tail;
    int node_num;
    int port;
    pthread_mutex_t mutex;
}T_Send_Av_Packet_Queue;
*/

int Initialize_recv_av_packet_queues(T_Recv_Av_Packet_Queue * * *qs, int base_id,int n);
int Insert_recv_av_packet_queue(T_Recv_Av_Packet_Queue *q, T_Recv_Av_Packet_Node *n);
int Remove_recv_av_packet_queue(T_Recv_Av_Packet_Queue *q, T_Recv_Av_Packet_Node **n);
int Check_recv_av_packet_queue_limit(T_Recv_Av_Packet_Queue *q);

int Initialize_decoded_av_frame_queues(T_Decoded_Av_Frame_Queue * * *qs, int base_id,int n);
int Insert_decoded_av_frame_queue(T_Decoded_Av_Frame_Queue *q, T_Decoded_Av_Frame_Node *n);
int Insert_decoded_av_frame_queue_wraper( void *queue, void *data );
int Remove_decoded_av_frame_queue(T_Decoded_Av_Frame_Queue *q, T_Decoded_Av_Frame_Node **n);
int Check_decoded_av_frame_queue_limit(T_Decoded_Av_Frame_Queue *q);

int Initialize_mixed_av_frame_queue(T_Mixed_Av_Frame_Queue **qs, int base_id,int n);
int Insert_mixed_av_frame_queue(T_Mixed_Av_Frame_Queue *q, T_Mixed_Av_Frame_Node *n);
int Insert_mixed_av_frame_queue_wraper(void *queue, void *data);
int Remove_mixed_av_frame_queue(T_Mixed_Av_Frame_Queue *q, T_Mixed_Av_Frame_Node **n);
int Check_mixed_av_frame_queue_limit(T_Mixed_Av_Frame_Queue *q );

int Initialize_encoded_av_packet_queue(T_Encoded_Av_Packet_Queue **qs,int base_id, int n);
int Insert_encoded_av_packet_queue(T_Encoded_Av_Packet_Queue *q, T_Encoded_Av_Packet_Node *n);
int Insert_encoded_av_packet_queue_wraper(void *queue, void *data);
int Remove_encoded_av_packet_queue(T_Encoded_Av_Packet_Queue *q, T_Encoded_Av_Packet_Node **n);
int Check_encoded_av_packet_queue_limit(T_Encoded_Av_Packet_Queue *q );

/* RTP format packet */
/*
int Initialize_send_av_packet_queues(T_Send_Av_Packet_Queue * *qs, int n);
int Insert_send_av_packet_queue(T_Send_Av_Packet_Queue *q, T_Send_Av_Packet_Node *n);
int Remove_send_av_packet_queue(T_Send_Av_Packet_Queue *q, T_Send_Av_Packet_Node **n);
int Check_send_av_packet_queue_limit(T_Send_Av_Packet_Queue *q);
*/
#endif   /* ----- #ifndef MANANGE_QUEUE_H_INC  ----- */
