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
#include "app_mengine.h"

//globle variable
//app_timer_vector��Ӧ����ʱ������ָ�룬ָ��Ӧ�ÿռ����ʱ������
app_timer_t mengine_app_timer_vector[APP_TIMER_COUNT];

#ifdef CUR_FILE_SWITCH
g_config_var_t g_config_var;
#endif

#ifndef PC
OS_STK *ptos = (OS_STK *) AP_MENGINE_STK_POS;
#endif
INT8U prio = AP_MENGINE_PRIO;

//�ı��ļ�·����Ϣ��ʶ
uint8 g_change_path_flag = 0;

//��ʼ��ʱ���̷�
uint8 g_open_disk;

// �����ļ��򿪱�־
uint8 g_music_fileopen_flag = 0;

//�м��״̬��ⶨʱ��
int8 g_status_timer;

//�м��״̬����־λ
uint8 g_check_status_flag;

//�м����ǰʱ��ȴ���ʱ,��ʱ3s��ǰʱ���ޱ仯�ж�Ϊ��֧��
uint8 g_wait_cur_time_change = 0;

//music engine��ʱ����
uint8 g_delay_time;

//set_file��־ TRUE OR FALSE
bool g_set_file_flag = FALSE;

//�ļ���buffer
uint8 g_file_name[64];

//sd �ļ���
//uint8 g_sd_file_name[12];

//�����м�����
void *mp_handle = NULL;

//����ļ�ϵͳid
int32 file_sys_id;

//����ģʽ
mengine_enter_mode_e mengine_enter_mode;

//������Ϣ
mengine_info_t mengine_info;

//���������ʼʱ��
uint32 g_intro_start;

//���music�ļ���׺����bitmap����
uint32 file_type_cfg;//

//ɾ����ǰ�ļ���ʶ
uint32 g_del_curfile;

//�ļ�·����Դ��Ϣ����,�ı�·����Ϣʱʹ��
fsel_type_e g_path_type;

//ϵͳ��������
setting_comval_t g_setting_comval;
//comval_t g_comval;

//��ǰ����ģʽ
play_mode_e g_play_mode;

//�ؼ�����ֵ
app_result_e g_mengine_result;

//�������ñ����ṹ��ָ��
mengine_config_t *g_eg_cfg_p;

//����״̬�����ṹ��ָ��
mengine_status_t *g_eg_status_p;

//���沥����Ϣ�ṹ��ָ��
mengine_playinfo_t *g_eg_playinfo_p;

//˽����Ϣȫ�ֱ���
private_msg_t g_cur_msg;

dae_config_t *g_dae_cfg_shm;

//for pre-read file data
mmm_prefetch_data_t *g_prefetch_data;


