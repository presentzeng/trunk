/*******************************************************************************
 *                              US212A
 *                            Module: MUSIC_MENGINE
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       yanghong  2016-1-28 15:37    1.0             build this file
 *******************************************************************************/
/*!
 * \file     *.c
 * \brief    ������д�ļ��ĸ���
 * \author   yanghong
 * \par      GENERAL DESCRIPTION:
 *               ������ļ���������
 * \par      EXTERNALIZED FUNCTIONS:
 *               �����������õ������ģ��
 * \version 1.0
 * \date  2016/1/28
 *******************************************************************************/

#ifndef _engine_rdata_h
#define _engine_rdata_h

#include "psp_includes.h"
#include "case_include.h"

//globle variable
//app_timer_vector��Ӧ����ʱ������ָ�룬ָ��Ӧ�ÿռ����ʱ������
extern app_timer_t mengine_app_timer_vector[APP_TIMER_COUNT];

#ifdef CUR_FILE_SWITCH
extern g_config_var_t g_config_var;
#endif

#ifndef PC
extern OS_STK *ptos;
#endif
extern INT8U prio;

//�ı��ļ�·����Ϣ��ʶ
extern uint8 g_change_path_flag;

//��ʼ��ʱ���̷�
extern uint8 g_open_disk;

//�����ļ��򿪱�־
extern uint8 g_music_fileopen_flag;

//�м��״̬��ⶨʱ��
extern int8 g_status_timer;

//�м��״̬����־λ
extern uint8 g_check_status_flag;

//�м����ǰʱ��ȴ���ʱ,��ʱ3s��ǰʱ���ޱ仯�ж�Ϊ��֧��
extern uint8 g_wait_cur_time_change;

//music engine��ʱ����
extern uint8 g_delay_time;

//set_file��־ TRUE OR FALSE
extern bool g_set_file_flag;

//�ļ���buffer
extern uint8 g_file_name[64];

//sd �ļ���
//extern uint8 g_sd_file_name[12];

//�����м�����
extern void *mp_handle;

//����ļ�ϵͳid
extern int32 file_sys_id;

//����ģʽ
extern mengine_enter_mode_e mengine_enter_mode;

//������Ϣ
extern mengine_info_t mengine_info;

//���������ʼʱ��
extern uint32 g_intro_start;

//���music�ļ���׺����bitmap����
extern uint32 file_type_cfg;

//ɾ����ǰ�ļ���ʶ
extern uint32 g_del_curfile;

//�ļ�·����Դ��Ϣ����,�ı�·����Ϣʱʹ��
extern fsel_type_e g_path_type;

//ϵͳȫ�ֱ���
extern setting_comval_t g_setting_comval;
//extern comval_t g_comval;

//��ǰ����ģʽ
extern play_mode_e g_play_mode;

//�ؼ�����ֵ
extern app_result_e g_mengine_result;

//�������ñ����ṹ��ָ��
extern mengine_config_t *g_eg_cfg_p;

//����״̬�����ṹ��ָ��
extern mengine_status_t *g_eg_status_p;

//���沥����Ϣ�ṹ��ָ��
extern mengine_playinfo_t *g_eg_playinfo_p;

//˽����Ϣȫ�ֱ���
extern private_msg_t g_cur_msg;

extern dae_config_t *g_dae_cfg_shm;

//for pre-read file data
extern mmm_prefetch_data_t *g_prefetch_data;

#endif //_engine_rdata_h
