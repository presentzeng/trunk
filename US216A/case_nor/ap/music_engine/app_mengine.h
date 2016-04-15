/********************************************************************************
 *                            Module: music_engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2011-09-05 15:00     1.0             build this file
 ********************************************************************************/
#ifndef __APP_MENGINE_H__
#define __APP_MENGINE_H__

#include "psp_includes.h"
#include "case_include.h"
#include "enhanced.h"

#include "music_common.h"
#include "setting_common.h"

//Ӧ���Լ���Ҫ����ʱ������
#define  APP_TIMER_COUNT		1

/*!�����������������ջ�����߳����ȼ�*/
#define  AP_MENGINE_STK_POS  AP_BACK_LOW_STK_POS
#define  AP_MENGINE_PRIO     AP_BACK_LOW_PRIO

/*�����л����жϵ㱣��궨��*/
#define CUR_FILE_SWITCH

/*������ŵ�ʱ��*/
#define  INTRO_INTERVAL 11000

/*��ǰʱ���ޱ仯�ȴ�ʱ��*/
#define WAIT_TIMEOUT   12 //12*250ms=3000ms
#define TIME_LIMIT     2000

#ifdef CUR_FILE_SWITCH
typedef struct
{
    uint16 magic;       //VM ����ħ��
    uint8 ap_id;        //�ػ���Ӧ��
    uint8 engine_state; //����״̬
    uint8 keyID[16];    //������������
} g_config_var_t;
#endif

//���ֲ���ģʽ
typedef enum
{
    //��������
    PLAY_NORMAL,
    //����
    PLAY_RESUME,
    //�������
    PLAY_FFON,
    //���˲���
    PLAY_FBON,
    //����Ҫ����
    PLAY_NO_PLAY
} play_mode_e;

//����ֹͣģʽ
typedef enum
{
    //����ֹͣ
    STOP_NORMAL,
    //��ͣ
    STOP_PAUSE
} stop_mode_e;

typedef enum
{
    NORMAL_SWITCH_NEXT = 0x0,
    NORMAL_SWITCH_PREV = 0x1,
    FORCE_SWITCH_NEXT  = 0x2,
    FORCE_SWITCH_PREV  = 0x3,
    END_SWITCH         = 0x4
} switch_mode_e;

typedef enum
{
    SWITCH_NO_ERR,
    SWITCH_ERR_STOP,
    SWITCH_ERR_SWITCH,
    SWITCH_ERR_PLAY
} switch_result_e;


/*!
 * \brief
 *  ������Ϣ�ṹ���������
 */ 
typedef struct
{
    /******************************************************************
     *          ����״̬��Ϣ
     *******************************************************************/
    mengine_status_t eg_status;

    /******************************************************************
     *          �����ļ���ʱ���bit����Ϣ
     *******************************************************************/
    mengine_playinfo_t eg_playinfo;

    /******************************************************************
     *          �м�������ļ���Ϣ
     *******************************************************************/
    mmm_mp_file_info_t eg_file_info;

    /******************************************************************

     *          ������Ϣ(��������)
     *******************************************************************/
    mengine_config_t eg_config;

} mengine_info_t;


//functions

extern app_result_e mengine_control_block(void)
__FAR__;

extern bool change_locat_deal(void)
__FAR__;

extern app_result_e mengine_message_done(private_msg_t* cur_msg)
__FAR__;

extern bool mengine_file_init(void)
__FAR__;

//extern void _end_switch(play_mode_e last_mode)
//__FAR__;

extern bool _set_file(void)
__FAR__;

extern bool _play(play_mode_e play_mode);

//__FAR__;

extern bool _stop(stop_mode_e stop_mode)
__FAR__;

//extern bool _next(bool force_mod)
//__FAR__;

//extern bool _prev(bool force_mod)
//__FAR__;

extern eg_err_e _convert_error_no(mmm_mp_err_t err_no)
__FAR__;

extern void _error_timeout_handle(void)
__FAR__;

//extern uint32 _volume_deal(uint8 vol_in, bool set_mode)
//__FAR__;

extern void mengine_save_errno_no(void)
__FAR__;
//for event
//extern app_result_e mengine_get_config(void* msg_ptr) __FAR__;

extern app_result_e mengine_get_playmode(void* msg_ptr)
__FAR__;

extern app_result_e mengine_get_filepath(void* msg_ptr)
__FAR__;

extern app_result_e mengine_get_fileinfo(void* msg_ptr)
__FAR__;

extern app_result_e mengine_get_status(void* msg_ptr)
__FAR__;

extern app_result_e mengine_get_playinfo(void* msg_ptr)
__FAR__;

extern app_result_e mengine_get_bkinfo(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_eq(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_volume(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_playmode(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_ab_count(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_ab_gap(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_ab_gap(void* msg_ptr)
__FAR__;

extern app_result_e mengine_start_ab(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_apoint(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_bpoint(void* msg_ptr)
__FAR__;

extern app_result_e mengine_clear_ab(void* msg_ptr)
__FAR__;

extern app_result_e mengine_switch(void* msg_ptr)
__FAR__;

extern app_result_e mengine_play(void* msg_ptr)
__FAR__;

extern app_result_e mengine_stop(void* msg_ptr)
__FAR__;

extern app_result_e mengine_pause(void* msg_ptr)
__FAR__;

extern app_result_e mengine_resume(void* msg_ptr)
__FAR__;

extern app_result_e mengine_fast_forward(void* msg_ptr)
__FAR__;

extern app_result_e mengine_fast_backward(void* msg_ptr)
__FAR__;

extern app_result_e mengine_fastplay_step(void *msg_ptr);

extern app_result_e mengine_cancel_ffb(void* msg_ptr)
__FAR__;

extern app_result_e mengine_play_next(void* msg_ptr)
__FAR__;

extern app_result_e mengine_play_prev(void* msg_ptr)
__FAR__;

extern app_result_e mengine_play_cuemusic(void* msg_ptr)
__FAR__;

extern app_result_e mengine_musui_delete_file(void* msg_ptr)
__FAR__;

extern app_result_e mengine_other_delete_file(void* msg_ptr)
__FAR__;

extern app_result_e mengine_filepath_deal(void* msg_ptr)
__FAR__;

extern app_result_e mengine_set_filepath(void* msg_ptr)
__FAR__;

extern app_result_e mengine_change_filepath(void* msg_ptr)
__FAR__;

extern app_result_e mengine_del_favorite_deal(void* msg_ptr)
__FAR__;

extern app_result_e mengine_add_favorite_deal(void* msg_ptr)
__FAR__;

extern app_result_e mengine_clear_error(void* msg_ptr)
__FAR__;

extern app_result_e mengine_message_done_bank(private_msg_t* cur_msg)
__FAR__;

extern void mengine_set_real_eq(uint8 mode)
__FAR__;

//extern app_result_e mengine_get_engine_info(void* msg_ptr)
//__FAR__;

extern void mengine_reply_msg(void* msg_ptr, bool ret_vals)
__FAR__;

extern switch_result_e mengine_file_switch(stop_mode_e stop_mode, switch_mode_e force_mode, play_mode_e play_mode)
__FAR__;

extern app_result_e mengine_standby_play(void* msg_ptr);

extern bool _mengine_start_sound(uint8 cur_volume);

extern bool _mengine_stop_sound(void);

extern bool _mengine_check_sd_in(void);

extern int _get_file_info(uint8 normal_file);

extern bool _file_switch_info(void);
//file io
extern int32 mmm_mp_read(void *buf, int32 size, int32 count, storage_io_t *io) __FAR__;
extern int32 mmm_mp_seek(storage_io_t *io, int32 offset, int32 whence)__FAR__;
extern int32 mmm_mp_tell(storage_io_t *io)__FAR__;
extern int32 mmm_mp_read_sd(void *buf, int32 size, int32 count, storage_io_t *io)__FAR__;
extern int32 mmm_mp_seek_sd(storage_io_t *io, int32 offset, int32 whence)__FAR__;
extern int32 mmm_mp_tell_sd(storage_io_t *io)__FAR__;

#endif //__APP_MENGINE_H__
