/*
 * =====================================================================================
 *
 *       Filename:  hvs_init.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/15/2010 9:26:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhi-wei YAN (Jerod YAN), jerod.yan@gmail.com
 *        Company:  DrumTm
 *
 * =====================================================================================
 */

#ifndef  HVS_INIT_H_INC
#define  HVS_INIT_H_INC

int Create_hybrid_video_process_x(int inpar_welcome_sd,struct sockaddr_in *inpar_sa);
int Create_hybrid_video_process(int inpar_welcome_sd,struct sockaddr_in *inpar_sa);

#endif   /* ----- #ifndef HVS_INIT_H_INC  ----- */
