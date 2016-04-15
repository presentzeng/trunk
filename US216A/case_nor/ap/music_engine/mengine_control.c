/*******************************************************************************
 *                              US211A
 *                            Module: music engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     fiona.yang     2011-09-06 15:00     1.0             build this file
 *******************************************************************************/
#include "app_mengine.h"
#include "mengine_rdata.h"
#include "dac_interface.h"
/******************************************************************************/
/*!
 * \par  Description:
 * \void mengine_status_deal(void)
 * \����״̬����,����ǰ���ֲ���״̬
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \ingroup      mengine_control.c
 * \note
 */
/*******************************************************************************/
void mengine_status_deal(void)
{
    mmm_mp_status_t mmm_status;
    bool bret = TRUE;
    bool err_ret = FALSE;
    uint32 music_total_time = (uint32)mengine_info.eg_file_info.total_time;
    
    //��ȡ��ǰ����״̬
    mmm_mp_cmd(mp_handle, MMM_MP_GET_STATUS, (unsigned int) &mmm_status);
    
    if ((mmm_status.status == MMM_MP_ENGINE_PLAYING) && //�м��Ϊ����״̬
        (g_eg_status_p->play_status == PlaySta) && //��̨Ϊ����״̬
        (mmm_status.cur_time > TIME_LIMIT) && //�ų���ͷ2s
        (mmm_status.cur_time < (music_total_time - TIME_LIMIT)) && //�ų�ĩβ2s
        (mmm_status.cur_time == g_eg_playinfo_p->cur_time))//��ǰʱ��û�б仯
    {
        g_wait_cur_time_change++;
        if (g_wait_cur_time_change >= WAIT_TIMEOUT)//ʱ�����12*250ms=3000ms
        {
            g_wait_cur_time_change = 0;
            err_ret = TRUE;
        }
    }
    else
    {
        g_wait_cur_time_change = 0;
    }
    //�ж��Ƿ�֧��HIFI
    if (g_setting_comval.music_comval.hifi_support == FALSE)
    {
        if (mengine_info.eg_file_info.sample_rate > 48)
        {
            err_ret = TRUE;
        }
    }
    
    if (err_ret == TRUE)
    {
        mmm_status.status = MMM_MP_ENGINE_ERROR;
    }
    
    //��ŵ�ǰ�Ĳ���ʱ��ͱ�����
    if ((g_eg_status_p->play_status != PauseSta) && (g_eg_status_p->play_status != StopSta))
    {
        if (mmm_status.status == MMM_MP_ENGINE_PLAYING)
        {
            g_eg_playinfo_p->cur_time = (uint32) mmm_status.cur_time;
        }
    }

    switch (mmm_status.status)
    {
        case MMM_MP_ENGINE_PLAYING://��������
        if (g_eg_status_p->play_status == PlaySta)//��������
        {
            //������������л�����
            if ((g_eg_cfg_p->repeat_mode == FSEL_MODE_INTRO) && 
                ((uint32) mmm_status.cur_time >= (g_intro_start + INTRO_INTERVAL)) && 
                (g_eg_cfg_p->cue_flag == 0x00))
            {
                mengine_file_switch(STOP_NORMAL, END_SWITCH, PLAY_NORMAL);
            }

            //�쳣�������
            if (g_eg_status_p->fast_status != FBPlay_Null)
            {
                mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 0);
                g_eg_status_p->fast_status = FBPlay_Null;
            }
        }
        else
        {
            //����˲�����������
            ;//do nothing
        }
        break;

        case MMM_MP_ENGINE_STOPPED://ֹͣ

        //sd �ļ�����ֻ��Ҫֹͣ
        if (g_eg_cfg_p->fsel_type == FSEL_TYPE_SDFILE)
        {
            _stop(STOP_NORMAL);//ֹͣ����,���ļ��ر�
            break;
        }

        //�������ŵ�β or �����β or ���˵�ͷֹͣ
        if ((g_eg_status_p->play_status == PlaySta) || (g_eg_status_p->play_status == PlayFast))
        {
            //���Ϊcue�������κ�̨�Զ��и�
            //��ǰ̨�����и趯��
            if(g_eg_cfg_p->cue_flag == 0x00)
            {
                
                if (g_eg_status_p->fast_status == FBPlay_Null)
                {
                    if (g_eg_playinfo_p->cur_time == music_total_time)
                    {
                        //�������ŵ�β
                        mengine_file_switch(STOP_NORMAL, END_SWITCH, PLAY_NORMAL);
                    }
                    else
                    {
                        //�ú���ÿ250msִ��һ��,ÿ�μ�250
                        g_eg_playinfo_p->cur_time += 250;
                        if (g_eg_playinfo_p->cur_time > music_total_time)
                        {
                            g_eg_playinfo_p->cur_time = music_total_time;
                        }
                    }
                }
                else if (g_eg_status_p->fast_status == FFPlaySta)
                {
                    //�����β
                    //music_print("#PLAY_FFON",0,0);
                    mengine_file_switch(STOP_NORMAL, END_SWITCH, PLAY_FFON);
                }
                else
                {
                    //���˵�β
                    //music_print("#PLAY_FBON",0,0);
                    mengine_file_switch(STOP_NORMAL, FORCE_SWITCH_PREV, PLAY_FBON);
                }
            }
        }
        break;

        case MMM_MP_ENGINE_ERROR://����

        //ǰ̨����music UI����ǰ̨��������״̬�����ִ���ֱ������һ��
        if ((g_app_info_state.app_state != APP_STATE_PLAYING) || (g_app_info_state.screensave_state == TRUE))
        {
            //ǰ̨����music ui,��ֱ������һ��
            _error_timeout_handle();
        }
        else
        {//����ȴ�ǰ̨��ӦUI״̬
        
            //���Ȼ�ȡ����ţ���Ϊstop���������ô����
            mengine_save_errno_no();//Ĭ��Ϊ�������

            bret = _stop(STOP_NORMAL);//ֹͣ����

            //�ָ���Ĭ��״̬,��ֹ�����������ʽ�����ļ��ٴο����
            g_eg_status_p->play_status = StopSta;
            g_eg_status_p->fast_status = FBPlay_Null;
        }
        break;

        default:
        break;
    }
    if ((g_eg_status_p->play_status == StopSta) || (g_eg_status_p->play_status == PauseSta))
    {
        g_app_info_state.state = ENGINE_STATE_PAUSE;
    }
    else
    {
        g_app_info_state.state = ENGINE_STATE_PLAYING;
    }
}
/******************************************************************************/
/*!
 * \par  Description:
 * \void _check_megine_status(void)
 * \�������״̬��ʱ�����������ñ�־λ
 * \param[in]    void
 * \param[out]   none
 * \return       none
 * \retval       none
 * \ingroup      mengine_control.c
 * \note
 */
/*******************************************************************************/
static void _check_megine_status(void)
{
    g_check_status_flag = TRUE;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \void mengine_control_block(void)
 * \���湦�ܴ�����
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \ingroup      mengine_control.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_control_block(void)
{
    g_mengine_result = RESULT_NULL;
    int ret = sys_drv_install(DRV_GROUP_I2C, 0, "drv_dac.drv");
    dac_open(0, 0, 0);
    //250ms��ѯһ���м��״̬
    g_status_timer = set_app_timer(APP_TIMER_ATTRB_CONTROL, 250, _check_megine_status);
    while (1)
    {
        //��ѯ��ǰ����״̬
        //if ((g_check_status_flag == TRUE) || (mengine_info.eg_status.play_status == PlayFast))
        if (g_check_status_flag == TRUE)
        {
            g_check_status_flag = FALSE;
            mengine_status_deal();
        }

        //��ȡ��������ϸ
        if (get_app_msg_for_engine(&g_cur_msg) == TRUE)
        {

            //music_print("mengine", g_cur_msg.msg.type,2);
        
            switch (g_cur_msg.msg.type)
            {
                /*��ȡ��Ϣ���*/
                case MSG_MENGINE_GET_PLAYMODE_SYNC://��ȡѭ��ģʽ+shuffle
                mengine_get_playmode(&g_cur_msg);
                break;

                case MSG_MENGINE_GET_FILEPATH_SYNC://��ȡ�ļ�location��Ϣ
                mengine_get_filepath(&g_cur_msg);
                break;

                case MSG_MENGINE_GET_FILEINFO_SYNC://��ȡ�ļ���ʽ��b/s����ʱ�����Ϣ
                mengine_get_fileinfo(&g_cur_msg);
                break;

                case MSG_MENGINE_GET_STATUS_SYNC://��ȡ��ǰ����״̬
                mengine_get_status(&g_cur_msg);
                break;

                case MSG_MENGINE_GET_PLAYINFO_SYNC://��ȡ��ǰ������Ϣ
                mengine_get_playinfo(&g_cur_msg);
                break;

                default:

		
                g_mengine_result = mengine_message_done_bank(&g_cur_msg);
                break;
            }
        }

        if (g_mengine_result == RESULT_APP_QUIT)
        {
            //�˳�Ӧ�ùرմ���
            _stop(STOP_NORMAL);
            break;
        }

        //����10ms �����񽻻�
        sys_os_time_dly(g_delay_time);
    }
    kill_app_timer(g_status_timer);
    return g_mengine_result;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_reply_msg(void* msg_ptr, bool ret_vals)
 * \�ظ�ͬ����Ϣ
 * \param[in]    void  msg_ptr ��Ϣ�ṹ��
 * \param[in]    bool  ret_vals ����¼������� TRUE �ɹ� FALSE ʧ��
 * \param[out]   none
 * \return       app_result_E
 * \retval       RESULT_IGNOR ����
 * \ingroup      mengine_control.c
 * \note  �ظ���Ϣʱ������ret_vals�Ľ��������Ӧ��ɹ���ʧ��
 */
/*******************************************************************************/
void mengine_reply_msg(void* msg_ptr, bool ret_vals)
{
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    if (ret_vals == TRUE)
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }

    //�ظ�ͬ����Ϣ(�����ź���)
    //reply_sync_msg(msg_ptr);
    libc_sem_post(data_ptr->sem);

    return;

    //return RESULT_IGNORE;
}


