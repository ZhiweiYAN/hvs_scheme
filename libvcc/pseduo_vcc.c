/*
 * =====================================================================================
 *
 *       Filename:  pseduo_lib_vcc.c
 *
 *    Description:
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
#include "pseduo_vcc.h"
#include <stdio.h>
#include <stdlib.h>
int Pseduo_Do_decode_packet( void *pkt, int(*f)(), void *decoded_frm_queue )
{
    int err = 0;
    int ret = 0;


    for(int i=0; i<5; i++) {

        void *frm = NULL;
        frm = malloc(10000);

        //put frms into the target queue.
        err = (*f)(decoded_frm_queue, frm);

    }

    //free packet of video
    free(pkt);
    pkt = NULL;
    return ret;
}


int Pseduo_Do_mix_frames( void **frms, int frm_n, int(*f)(), void *mixed_frm_queue )
{
    int err = 0;
    int ret = 0;

    //mixed frames, if NULL==frm[i], fill the blue
    void *frm = NULL;
    frm = malloc(10000);

    //put frame into the target queue.
    err = (*f)(mixed_frm_queue, (void*)frm);

    //free frames of video
    for(int i=0; i<frm_n; i++) {
		free(frms[i]);
		frms[i] = NULL;
    }

    return ret;
}

int Pseduo_Do_encode_frame(void *frm, int(*f)(), void *encoded_pkt_queue)
{
    int err = 0;
    int ret = 0;

    //after encoding, a new packet is created.
    void *pkt = NULL;
	pkt = malloc( 10000 );

    //put pkts into the target queue.
    err = (*f)(encoded_pkt_queue, (void *)pkt);

    free(frm);
    frm = NULL;

    return ret;
}

