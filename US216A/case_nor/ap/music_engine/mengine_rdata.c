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
 * \brief    这里填写文件的概述
 * \author   yanghong
 * \par      GENERAL DESCRIPTION:
 *               这里对文件进行描述
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 * \version 1.0
 * \date  2016/1/28
 *******************************************************************************/
#include "app_mengine.h"

//globle variable
//app_timer_vector：应用软定时器数组指针，指向应用空间的软定时器数组
app_timer_t mengine_app_timer_vector[APP_TIMER_COUNT];

#ifdef CUR_FILE_SWITCH
g_config_var_t g_config_var;
#endif

#ifndef PC
OS_STK *ptos = (OS_STK *) AP_MENGINE_STK_POS;
#endif
INT8U prio = AP_MENGINE_PRIO;

//改变文件路径信息标识
uint8 g_change_path_flag = 0;

//初始化时的盘符
uint8 g_open_disk;

// 音乐文件打开标志
uint8 g_music_fileopen_flag = 0;

//中间件状态检测定时器
int8 g_status_timer;

//中间件状态检测标志位
uint8 g_check_status_flag;

//中间件当前时间等待超时,超时3s当前时间无变化判断为不支持
uint8 g_wait_cur_time_change = 0;

//music engine延时参数
uint8 g_delay_time;

//set_file标志 TRUE OR FALSE
bool g_set_file_flag = FALSE;

//文件名buffer
uint8 g_file_name[64];

//sd 文件名
//uint8 g_sd_file_name[12];

//保存中间件句柄
void *mp_handle = NULL;

//存放文件系统id
int32 file_sys_id;

//进入模式
mengine_enter_mode_e mengine_enter_mode;

//引擎信息
mengine_info_t mengine_info;

//浏览播放起始时间
uint32 g_intro_start;

//存放music文件后缀名的bitmap配置
uint32 file_type_cfg;//

//删除当前文件标识
uint32 g_del_curfile;

//文件路径来源信息保存,改变路径信息时使用
fsel_type_e g_path_type;

//系统公共变量
setting_comval_t g_setting_comval;
//comval_t g_comval;

//当前播放模式
play_mode_e g_play_mode;

//控件返回值
app_result_e g_mengine_result;

//引擎配置变量结构体指针
mengine_config_t *g_eg_cfg_p;

//引擎状态变量结构体指针
mengine_status_t *g_eg_status_p;

//引擎播放信息结构体指针
mengine_playinfo_t *g_eg_playinfo_p;

//私有消息全局变量
private_msg_t g_cur_msg;

dae_config_t *g_dae_cfg_shm;

//for pre-read file data
mmm_prefetch_data_t *g_prefetch_data;


