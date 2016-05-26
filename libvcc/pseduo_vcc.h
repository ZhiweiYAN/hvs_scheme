/*
 * =====================================================================================
 *
 *       Filename:  pseduo_lib_vcc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  4/7/2011
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  PSEDUO_LIB_VCC_H_INC
#define  PSEDUO_LIB_VCC_H_INC

int Pseduo_Do_decode_packet( void *pkt, int(*f)() , void *decoded_frm_queue ) __attribute__((cdecl));
int Pseduo_Do_mix_frames( void **frms, int frm_n, int(*f)(), void *mixed_frm_queue ) __attribute__((cdecl));
int Pseduo_Do_encode_frame(void *frm, int(*f)(), void *encoded_pkt_queue) __attribute__((cdecl));

#endif   /* ----- #ifndef PSEDUO_LIB_VCC_H_INC  ----- */
