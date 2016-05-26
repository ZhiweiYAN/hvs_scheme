/*
 * =====================================================================================
 *
 *       Filename:  manage_queue.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/9/2010 5:50:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */
#include "manage_queue.h"

/* ********************************************
 * Return values:
 * 	0 : the input parameters are invalid.
 * 	-1: there are something wrong.
 * 	1 : everything is good.
 * 	******************************************* */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_recv_av_packet_queues
 *  Description:  None.
 * =====================================================================================
 */
int Initialize_recv_av_packet_queues(T_Recv_Av_Packet_Queue * * *qs, int base_id, int n)
{
    assert(NULL!=qs);
    assert(n>=1);

    int err = 0;

    if (NULL!=*qs||n<1) {
        OUTPUT_ERROR;
        return 0;
    }

    //create n pointer of queues of Recv Av Packet.
    *qs = (T_Recv_Av_Packet_Queue **)malloc(sizeof(void *)*n);
    assert(NULL!=*qs);

    if (NULL==*qs) {
        OUTPUT_ERROR;
        return -1;
    }
    bzero(*qs, sizeof(void *)*n);

    T_Recv_Av_Packet_Queue **queues = *qs;

    //create n queues of Recv Av Packet
    for (int i=0; i<n; i++) {
        queues[i] =  (T_Recv_Av_Packet_Queue *) malloc( sizeof(T_Recv_Av_Packet_Queue) );
        if (NULL==queues[i]) {
            OUTPUT_ERROR;
            return -1;
        }
        assert(NULL!=queues[i]);
        bzero(queues[i], sizeof(T_Recv_Av_Packet_Queue));
        err = pthread_mutex_init( &((queues[i])->mutex), NULL);
        queues[i]->port = RECV_AV_QUEUE_PORT+ (i+1)*10;
        (queues[i]->q_info).id = base_id + i;
        (queues[i]->q_info).type = RECEIVE_QUEUE;
    }

    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Check_recv_av_packet_queue_limit
 *  Description:  if the length exceeds the limit, it returns -1;
 * =====================================================================================
 */
int Check_recv_av_packet_queue_limit(T_Recv_Av_Packet_Queue *q)
{
    assert( NULL!= q );

    int len = 0;
    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }
    err = pthread_mutex_lock(&(q->mutex));
    len = (q->q_info).length;
    err = pthread_mutex_unlock(&(q->mutex));

    if (MAX_AV_PACKET_NUM <= len) {
        DBG("%s:%d\n","The queue of AV packets reaches the length limit",len);
        return -1;
    } else {
        return 1;
    }
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  int Insert_recv_av_queue
 *  Description:  Insert a Av_packet node into the receiving queue
 * =====================================================================================
 */
int Insert_recv_av_packet_queue(T_Recv_Av_Packet_Queue *q, T_Recv_Av_Packet_Node *n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));
    //if the queue is empty, or else ...
    if (NULL == q->header) {

        q->header = n;
        q->tail = n;
        (q->q_info.length) ++;

    } else {

        n->next = NULL;
        q->tail->next = (struct T_Recv_Av_Packet_Node *)n;
        q->tail = n;
        (q->q_info.length) ++;
    }

    DBG("After INSERT, there are %d nodes in T_Recv_Av_Packet_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));
    usleep(10);
    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  int Remove_recv_av_queue
 *  Description:  Fetch a Av_packet node from the queue
 * =====================================================================================
 */
int Remove_recv_av_packet_queue(T_Recv_Av_Packet_Queue *q, T_Recv_Av_Packet_Node **n)
{

    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if recv node queue is empty
    if (NULL == q->header) {
        //OUTPUT_ERROR;
        err = pthread_mutex_unlock(&(q->mutex));
        return -1;
    } else {

        //fetch a node from the queue
        *n = q->header;

        (q->q_info.length) --;

        if (q->tail  == q->header) {

            q->header = NULL;
            q->tail = NULL;
        } else {
            q->header = (T_Recv_Av_Packet_Node *)(q->header->next);
        }
    }

    DBG("After REMOVE, there are %d nodes in T_Recv_Av_Packet_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));

    usleep(10);

    return 1;

}




/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_decoded_yuv_frame_queues
 *  Description:  None.
 * =====================================================================================
 */
int Initialize_decoded_av_frame_queues(T_Decoded_Av_Frame_Queue ** *qs, int base_id, int n)
{
    assert(NULL!=qs);
    assert(n>=1);

    int err = 0;

    if (NULL!=*qs||n<1) {
        OUTPUT_ERROR;
        return 0;
    }

    *qs = (T_Decoded_Av_Frame_Queue * *)malloc(sizeof(void *)*n);
    if (NULL==*qs) {
        OUTPUT_ERROR;
        return -1;
    }
    bzero(*qs, sizeof(void *)*n);

    T_Decoded_Av_Frame_Queue **queues = *qs;

    for (int i=0; i<n; i++) {
        queues[i] =  (T_Decoded_Av_Frame_Queue *) malloc( sizeof(T_Decoded_Av_Frame_Queue) );
        if (NULL==queues[i]) {
            OUTPUT_ERROR;
            return -1;
        }
        assert(NULL!=queues[i]);
        bzero(queues[i], sizeof(T_Decoded_Av_Frame_Queue));
        err = pthread_mutex_init( &((queues[i])->mutex), NULL);
        (queues[i]->q_info).id = base_id + i;
        (queues[i]->q_info).type = DECODED_QUEUE;
    }

    return 1;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Check_decoded_av_frame_queue_limit
 *  Description:  if the length exceeds the limit, it returns -1;
 * =====================================================================================
 */
int Check_decoded_av_frame_queue_limit(T_Decoded_Av_Frame_Queue *q)
{
    int len = 0;
    int err = 0;

    assert( NULL!= q );

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }
    err = pthread_mutex_lock(&(q->mutex));
    len = (q->q_info).length;
    err = pthread_mutex_unlock(&(q->mutex));

    if (MAX_AV_FRAME_NUM <= len) {
        DBG("%s:%d\n","The queue of AV frames reaches the length limit",len);
        return -1;
    } else {
        return 1;
    }
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  int Insert_decoded_av_frame_queue_wrraper(void *q, void *av_frame)
 *  Description:  
 * =====================================================================================
 */
 int Insert_decoded_av_frame_queue_wraper(void *queue, void *data )
{
	assert(NULL!=queue);
	assert(NULL!=data);
	
	int err = 0;
	int ret = 0;

	T_Decoded_Av_Frame_Queue *av_frm_q = NULL;	
	void *av_frm = NULL;
	
	av_frm_q = (T_Decoded_Av_Frame_Queue *)queue;
	av_frm = data;

	T_Decoded_Av_Frame_Node *av_frm_node = NULL;
    av_frm_node = malloc(sizeof(T_Decoded_Av_Frame_Node));
    assert(NULL!=av_frm_node);
    bzero(av_frm_node, sizeof(T_Decoded_Av_Frame_Node) );
    av_frm_node->av_frame = av_frm; 

	err = Insert_decoded_av_frame_queue(av_frm_q, av_frm_node);
	
	if (-1==err){
		ret = -1;
	}
	else{
		ret = 1;
	}

	return ret;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Insert_decoded_av_frame_queue
 *  Description:  Insert a Av_frame node into the queue
 * =====================================================================================
 */
int Insert_decoded_av_frame_queue(T_Decoded_Av_Frame_Queue *q, T_Decoded_Av_Frame_Node *n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if the queue is empty, or else ...
    if (NULL == q->header) {

        q->header = n;
        q->tail = n;
        (q->q_info.length) ++;

    } else {

        n->next = NULL;
        q->tail->next = (struct T_Decoded_Av_Frame_Node *)n;
        q->tail = n;
        (q->q_info.length) ++;

    }

    DBG("After INSERT, there are %d nodes in T_Decoded_Av_Frame_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));

    usleep(10);

    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Remove_decoded_yuv_frame_queue
 *  Description:  Fetch a Av_frame node from the queue
 * =====================================================================================
 */
int Remove_decoded_av_frame_queue(T_Decoded_Av_Frame_Queue *q, T_Decoded_Av_Frame_Node **n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if recv node queue is empty, or else...
    if (NULL == q->header) {
        err = pthread_mutex_unlock(&(q->mutex));
        //OUTPUT_ERROR;
        return -1;
    } else {

        //fetch a node from the queue
        *n = q->header;

        (q->q_info.length) --;

        if (q->tail  == q->header) {

            q->header = NULL;
            q->tail = NULL;
        } else {
            q->header = (T_Decoded_Av_Frame_Node *)(q->header->next);
        }
    }
    DBG("After REMOVE, there are %d nodes in T_Decoded_Av_Frame_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));
    usleep(10);
    return 1;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_mixed_av_frame_queue
 *  Description:
 * =====================================================================================
 */
int Initialize_mixed_av_frame_queue(T_Mixed_Av_Frame_Queue **qs,int base_id, int n)
{

    assert(NULL!=qs);
    assert(n>=1);

    int err = 0;

    if (NULL!=*qs || n<1) {
        OUTPUT_ERROR;
        return -1;
    }
    *qs = (T_Mixed_Av_Frame_Queue * )malloc(sizeof(T_Mixed_Av_Frame_Queue));
    assert(NULL!=*qs);
    if (NULL==*qs) {
        OUTPUT_ERROR;
        return -1;
    }
    bzero(*qs, sizeof(T_Mixed_Av_Frame_Queue));

    //ID modification
    (*qs)->q_info.id = base_id;
    (*qs)->q_info.type = MIXED_QUEUE;

    err = pthread_mutex_init ( &((*qs)->mutex), NULL );

    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Check_mixed_av_frame_queue
 *  Description:
 * =====================================================================================
 */
int Check_mixed_av_frame_queue_limit(T_Mixed_Av_Frame_Queue *q )
{
    int len = 0;
    int err = 0;
    assert( NULL!= q );

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }
    err = pthread_mutex_lock(&(q->mutex));
    len = (q->q_info).length;
    err = pthread_mutex_unlock(&(q->mutex));

    if (MAX_AV_FRAME_NUM <= len) {
        DBG("%s:%d\n","The queue of AV packets reaches the length limit",len);
        return -1;
    } else {
        return 1;
    }
}

 int Insert_mixed_av_frame_queue_wraper(void *queue, void *data)
{
	assert(NULL!=data);
	
	int err = 0;
	int ret = 0;

	T_Mixed_Av_Frame_Queue *av_frm_q = NULL;
	av_frm_q = (T_Mixed_Av_Frame_Queue *)queue;
	void *av_frm = NULL;
	av_frm = (void *)data;

	T_Mixed_Av_Frame_Node *av_frm_node = NULL;
    av_frm_node = malloc(sizeof(T_Mixed_Av_Frame_Node));
    assert(NULL!=av_frm_node);
    bzero(av_frm_node, sizeof(T_Mixed_Av_Frame_Node) );
    av_frm_node->av_frame = av_frm; 

	err = Insert_mixed_av_frame_queue(av_frm_q, av_frm_node);
	
	if (-1==err){
		ret = -1;
	}
	else{
		ret = 1;
	}

	return ret;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Insert_mixed_av_frame_queue
 *  Description:
 * =====================================================================================
 */
int Insert_mixed_av_frame_queue(T_Mixed_Av_Frame_Queue *q, T_Mixed_Av_Frame_Node *n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;
    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if the queue is empty, or else ...
    if (NULL == q->header) {

        q->header = n;
        q->tail = n;
        (q->q_info.length) ++;

    } else {

        n->next = NULL;
        q->tail->next = (struct T_Mixed_Av_Frame_Node *)n;
        q->tail = n;
        (q->q_info.length) ++;

    }

    DBG("After INSERT, there are %d nodes in T_Mixed_Av_Frame_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));

    usleep(10);

    return 1;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Remove_mixed_av_frame_queue
 *  Description:
 * =====================================================================================
 */
int Remove_mixed_av_frame_queue(T_Mixed_Av_Frame_Queue *q, T_Mixed_Av_Frame_Node **n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if recv node queue is empty, or else...
    if (NULL == q->header) {
        pthread_mutex_unlock(&(q->mutex));
        //OUTPUT_ERROR;
        return -1;
    } else {

        //fetch a node from the queue
        *n = q->header;

        (q->q_info.length) --;

        if (q->tail  == q->header) {

            q->header = NULL;
            q->tail = NULL;
        } else {
            q->header = (T_Mixed_Av_Frame_Node *)(q->header->next);
        }
    }
    DBG("After REMOVE, there are %d nodes in T_Mixed_Av_Frame_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));
    usleep(10);
    return 1;

}



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_encoded_av_packet_queue
 *  Description:
 * =====================================================================================
 */
int Initialize_encoded_av_packet_queue(T_Encoded_Av_Packet_Queue **qs, int base_id, int n)
{
    assert(NULL!=qs);
    assert(n>=1);

    int err = 0;

    if (NULL!=*qs || n<1) {
        OUTPUT_ERROR;
        return -1;
    }
    *qs = (T_Encoded_Av_Packet_Queue* )malloc(sizeof(T_Encoded_Av_Packet_Queue));
    assert(NULL!=*qs);
    if (NULL==*qs) {
        OUTPUT_ERROR;
        return -1;
    }
    bzero(*qs, sizeof(T_Encoded_Av_Packet_Queue));

    err = pthread_mutex_init ( &((*qs)->mutex), NULL );

    //ID modification
    (*qs)->q_info.id = base_id;
    (*qs)->q_info.type = ENCODED_QUEUE;

    return 1;

}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Check_encoded_av_packet_queue
 *  Description:
 * =====================================================================================
 */
int Check_encoded_av_packet_queue_limit(T_Encoded_Av_Packet_Queue *q )
{
    int len = 0;
    int err = 0;
    assert( NULL!= q );

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }
    err = pthread_mutex_lock(&(q->mutex));
    len = (q->q_info).length;
    err = pthread_mutex_unlock(&(q->mutex));

    if (MAX_AV_PACKET_NUM <= len) {
        DBG("%s:%d\n","The queue of AV packets reaches the length limit",len);
        return -1;
    } else {
        return 1;
    }
}

 int Insert_encoded_av_packet_queue_wraper(void *queue, void *data)
{
	assert(NULL!=data);
	
	int err = 0;
	int ret = 0;

	T_Encoded_Av_Packet_Queue *av_pkt_q = NULL;
	av_pkt_q = (T_Encoded_Av_Packet_Queue *)queue;
	void *av_pkt = NULL;
	av_pkt = (void *)data;

	T_Encoded_Av_Packet_Node *av_pkt_node = NULL;
    av_pkt_node = malloc(sizeof(T_Encoded_Av_Packet_Node));
    assert(NULL!=av_pkt_node);
    bzero(av_pkt_node, sizeof(T_Encoded_Av_Packet_Node) );
    av_pkt_node->av_pkt = av_pkt;
	
	err = Insert_encoded_av_packet_queue(av_pkt_q, av_pkt_node);
	
	if (-1==err){
		ret = -1;
	}
	else{
		ret = 1;
	}

	return ret;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Insert_encoded_av_packet_queue
 *  Description:
 * =====================================================================================
 */
int Insert_encoded_av_packet_queue(T_Encoded_Av_Packet_Queue *q, T_Encoded_Av_Packet_Node *n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;
    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if the queue is empty, or else ...
    if (NULL == q->header) {

        q->header = n;
        q->tail = n;
        (q->q_info.length) ++;

    } else {

        n->next = NULL;
        q->tail->next = (struct T_Encoded_Av_Packet_Node *)n;
        q->tail = n;
        (q->q_info.length) ++;

    }

    DBG("After INSERT, there are %d nodes in T_Encoded_AV_Packet_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));

    usleep(10);

    return 1;

}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  Remove_encoded_av_packet_queue
 *  Description:
 * =====================================================================================
 */
int Remove_encoded_av_packet_queue(T_Encoded_Av_Packet_Queue *q, T_Encoded_Av_Packet_Node **n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if recv node queue is empty, or else...
    if (NULL == q->header) {
        pthread_mutex_unlock(&(q->mutex));
        //OUTPUT_ERROR;
        return -1;
    } else {

        //fetch a node from the queue
        *n = q->header;

        (q->q_info.length) --;

        if (q->tail  == q->header) {

            q->header = NULL;
            q->tail = NULL;
        } else {
            q->header = (T_Encoded_Av_Packet_Node *)(q->header->next);
        }
    }
    DBG("After REMOVE, there are %d nodes in T_Encoded_AV_Packet_Queue (ID=%d).\n",
        q->q_info.length,q->q_info.id);
    err = pthread_mutex_unlock(&(q->mutex));
    usleep(10);
    return 1;

}

/*
int Initialize_send_av_packet_queues(T_Send_Av_Packet_Queue * *qs, int n)
{
	assert(NULL!=qs);
	assert(n>=1);

	int err = 0;

	if (NULL!=*qs || n<1) {
		OUTPUT_ERROR;
		return -1;
	}
	*qs = (T_Send_Av_Packet_Queue* )malloc(sizeof(T_Send_Av_Packet_Queue));
	assert(NULL!=*qs);
	if(NULL==*qs){
		OUTPUT_ERROR;
		return -1;
		}
    bzero(*qs, sizeof(T_Send_Av_Packet_Queue));

    err = pthread_mutex_init ( &((*qs)->mutex), NULL );

	return 1;
}

int Insert_send_av_packet_queue(T_Send_Av_Packet_Queue *q, T_Send_Av_Packet_Node *n)
{
	assert(NULL!=q);
    assert(NULL!=n);

	int err = 0;
    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if the queue is empty, or else ...
    if (NULL == q->header) {

        q->header = n;
        q->tail = n;
        q->node_num ++;

    } else {

        n->next = NULL;
        q->tail->next = (struct T_Encoded_Av_Packet_Node *)n;
        q->tail = n;
        q->node_num ++;

    }

    DBG("After INSERT, there are %d nodes in T_Send_Av_Packet_Queue (ID=%d).\n", q->node_num,q->id);
    err = pthread_mutex_unlock(&(q->mutex));

    usleep(10);

    return 1;

}
int Remove_send_av_packet_queue(T_Send_Av_Packet_Queue *q, T_Send_Av_Packet_Node **n)
{
    assert(NULL!=q);
    assert(NULL!=n);

    int err = 0;

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }

    err = pthread_mutex_lock(&(q->mutex));

    //if recv node queue is empty, or else...
    if (NULL == q->header) {
        pthread_mutex_unlock(&(q->mutex));
        //OUTPUT_ERROR;
        return -1;
    } else {

        //fetch a node from the queue
        *n = q->header;

        q->node_num --;

        if (q->tail  == q->header) {

            q->header = NULL;
            q->tail = NULL;
        } else {
            q->header = (T_Encoded_Av_Packet_Node *)(q->header->next);
        }
    }
    DBG("After INSERT, there are %d nodes in T_Send_Av_Packet_Queue (ID=%d).\n", q->node_num,q->id);
    err = pthread_mutex_unlock(&(q->mutex));
    usleep(10);
    return 1;
}


int Check_send_av_packet_queue_limit(T_Send_Av_Packet_Queue *q)
{
    int n = 0;
	int err = 0;
    assert( NULL!= q );

    if (NULL==q) {
        OUTPUT_ERROR;
        return 0;
    }
    err = pthread_mutex_lock(&(q->mutex));
    n = q->node_num;
    err = pthread_mutex_unlock(&(q->mutex));

    if (MAX_AV_PACKET_NUM <= n) {
        DBG("%s:%d\n","The queue of AV packets reaches the length limit",n);
        return -1;
    } else {
        return 1;
    }
}*/
