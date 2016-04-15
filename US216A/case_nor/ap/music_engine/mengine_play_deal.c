/*******************************************************************************
 *                              US212A
 *                            Module: music engine
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      fiona.yang  2012-03-02        1.0              create this file
 *******************************************************************************/

#include "app_mengine.h"
#include "mengine_rdata.h"
#include "Dac_interface.h"

static mmm_mp_status_t mmm_status _BANK_DATA_ATTR_;

/******************************************************************************/
/*!
 * \par  Description:
 * \void _file_switch_info(void)
 * \�л���һ�����ļ���Ϣ��ȡ
 * \param[in]    play_mode_e para1
 * \param[in]    last_mode:

 * \param[out]   none
 * \return       the result
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
bool _file_switch_info(void)
{
    bool ret = TRUE;
    
    //��ȡ�ļ���Ϣ���粥����ʱ��
    ret = _set_file();
    if (ret == 0)
    {
        g_set_file_flag = FALSE;
    }
    //ͨ��SET_FILE���������Ƿ���ȷ������FILE_SWITCH_INFO,�����Ĵ����
    //��Ҫ��������
    //if(g_eg_status_p->err_status != EG_ERR_NONE)
    //{
    g_eg_status_p->err_status = EG_ERR_NONE;
    //}
    
    if (mengine_enter_mode == ENTER_START)
    {
        //�ر��ļ�
        mmm_mp_cmd(mp_handle, MMM_MP_CLEAR_FILE, (unsigned int) NULL);
    }

    return ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool _set_file(void)
 * \���ò����ļ���������
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
bool _set_file(void)
{
    int result, m_type, level_a;

    //set_file���������ñ�־��λ
    g_set_file_flag = TRUE;

    //���ȼ�⿨�ļ��Ƿ����
    if (_mengine_check_sd_in() == FALSE)
    {
        return FALSE;
    }

    //�ļ�ϵͳ����id
    if (g_eg_cfg_p->fsel_type == FSEL_TYPE_SDFILE)
    {
        result = _get_file_info(FALSE);
    }
    else
    {
        //�������ʱ�־
        g_eg_playinfo_p->cur_lyric = 0;
        vfs_get_name(file_sys_id, g_file_name, 32);
        g_eg_playinfo_p->cur_lyric = lyric_open(g_file_name);

        result = _get_file_info(TRUE);
    }

    if (result != 0)
    {
        mengine_save_errno_no();
        return FALSE;
    }

    m_type = mengine_info.eg_file_info.music_type;
    //music_print("type:", m_type,2);

    //their add dae check
    if (m_type == MP3_TYPE)
    {
        m_type = FREQ_LEVEL6; //
        level_a = FREQ_LEVEL7;
    }
    else if (m_type == WAV_TYPE)
    {
        m_type = FREQ_LEVEL6; //
        level_a = FREQ_LEVEL7;
    }
    else if ((m_type == APE_TYPE) || (m_type == FLAC_TYPE))
    {
        m_type = FREQ_LEVEL10; //
        level_a = FREQ_LEVEL9;
    }
    else if (m_type == WMA_TYPE)
    {
        m_type = FREQ_LEVEL6; //
        level_a = FREQ_LEVEL7;
    }
    else
    {
        m_type = FREQ_LEVEL10; //
        level_a = FREQ_LEVEL9;
    }

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, level_a, m_type);

    return TRUE;

}

  	
/******************************************************************************/
/*!
 * \par  Description:
 * \bool _play(play_mode_e play_mode)
 * \��ʼ��������
 * \param[in]    play_mode ����ģʽ para1
 * \param[in]    play_mode_e:
 *							 \PLAY_NORMAL ������ʼ����
 *							 \PLAY_RESUME �ϵ�����
 *							 \PLAY_FFON   �������
 *							 \PLAY_FBON   ���˲���
 * \param[out]   none
 * \return       bool the result
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 * \li   ע������ļ�SET_FILEʧ�ܣ����ﲢ��ֱ�ӹرվ��,������ͨ��״̬����ȡ����
 �ţ�Ȼ���ٹرվ�������ֱ�ӵ���CLEAR_FILE�������������
 */
/*******************************************************************************/
bool _play(play_mode_e play_mode)
{
    int result;
    bool bret = TRUE;
    int aout_mode = (aout_type_e) com_get_config_default(SETTING_AUDIO_OUTPUT_CHANNAL);
    int  pa_mode =  (uint8)com_get_config_default(SETTING_AUDIO_INNER_PA_SET_MODE);
    //if((1 == aout_mode) && (4 == pa_mode))
  //  {
      	//dac_open(0, 0, 0);
    //}
    //ֹͣ or ��ͣʱ ���Ͳ�������
    if ((mengine_info.eg_status.play_status == StopSta) || (mengine_info.eg_status.play_status == PauseSta))
    {
        if (g_set_file_flag == FALSE)
        {
            bret = _set_file();
        }
        //PLAY����Ҫ�����־λ
        g_set_file_flag = FALSE;
        
        if (bret == FALSE)
        {
            goto end_play;
        }
        com_set_sound_out(TRUE, SOUND_OUT_RESUME, _mengine_start_sound);

        //�п��ܴӲ�֧�ֲ��ŵ������л���֧�ֲ��ŵ����͵�EQ,�����Ҫ����
        mengine_set_real_eq(TRUE);

        if (play_mode == PLAY_FFON)
        {
            //�������
            result = mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 4);
            mengine_info.eg_status.play_status = PlayFast;//���ò���״̬
        }
        else if (play_mode == PLAY_FBON)
        {
            //���˲���
            mengine_info.eg_config.bk_infor.bp_time_offset = -5000;
            mengine_info.eg_config.bk_infor.bp_file_offset = 0;
            mengine_info.eg_config.bk_infor.bp_info_ext = 0;
            mmm_mp_cmd(mp_handle, MMM_MP_SET_BREAK_POINT, (uint32) (&(mengine_info.eg_config.bk_infor)));

            result = mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) -4);
            mengine_info.eg_status.play_status = PlayFast;//���ò���״̬
        }
        else if (play_mode == PLAY_RESUME)
        {
            //�ϵ�����,���öϵ�
            mmm_mp_cmd(mp_handle, MMM_MP_SET_BREAK_POINT, (uint32) (&(mengine_info.eg_config.bk_infor)));
            mengine_info.eg_status.play_status = PlaySta;//���ò���״̬
        }
        else
        {
            mengine_info.eg_status.play_status = PlaySta;//���ò���״̬
        }

        //���Ͳ�������
        result = mmm_mp_cmd(mp_handle, MMM_MP_PLAY, (uint32) NULL);
        if (result != 0)
        {
            bret = FALSE;
        }
        else
        {
            if (libc_memcmp(mengine_info.eg_config.location.dirlocation.filename, "AAC", 4) == 0)
            {
                //��ȡ�ļ���ʽ����ʱ�䡢�����ʵ���Ϣ
                result = mmm_mp_cmd(mp_handle, MMM_MP_MEDIA_INFO, (uint32) & mengine_info.eg_file_info);
            }
            bret = TRUE;
        }
        //�������,���ü�ʱ��ʼʱ��
        if (mengine_info.eg_config.repeat_mode == FSEL_MODE_INTRO)
        {
            mmm_mp_cmd(mp_handle, MMM_MP_GET_STATUS, (unsigned int) &mmm_status);
            g_intro_start = (uint32) mmm_status.cur_time;
        }

    }
    end_play: if (bret == FALSE)
    {
        mengine_save_errno_no();
    }
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool _stop(stop_mode_e stop_mode)
 * \ֹͣ��������
 * \param[in]    stop_mode ֹͣģʽ para1
 * \param[in]    stop_mode_e:
 * 							 \STOP_NORMAL ����ֹͣ
 * 							 \STOP_PAUSE  ��ͣ����
 * \param[out]   none
 * \return       bool the result
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
bool _stop(stop_mode_e stop_mode)
{
    bool bret = TRUE;
    int result = 0;

    //mengine_config_t *cfg_ptr = &mengine_info.eg_config;
    mengine_status_t *eg_status = &mengine_info.eg_status;

    //ȡ������� �ص���������״̬
    if (eg_status->play_status == PlayFast)
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 0);
        eg_status->play_status = PlaySta;
    }
    //�������ŲŻ�ֹͣ
    if (eg_status->play_status == PlaySta)
    {
        //����ϵ�
        mmm_mp_cmd(mp_handle, MMM_MP_GET_BREAK_POINT, (unsigned int) &(mengine_info.eg_config.bk_infor));
        //ֹͣ����
        result = mmm_mp_cmd(mp_handle, MMM_MP_STOP, (unsigned int) NULL);
        if (result != 0)
        {
            bret = FALSE;
        }
        else
        {
            //��ͣ����
            if (stop_mode == STOP_PAUSE)
            {
                eg_status->play_status = PauseSta;
            }
            else
            {
                //����ֹͣ
                eg_status->play_status = StopSta;
                mengine_info.eg_config.file_cnt.num = 1;
            }
            bret = TRUE;
        }

        if (bret == FALSE)
        {
            mengine_save_errno_no();
        }

        //�ر���Ƶ���
        com_set_sound_out(FALSE, SOUND_OUT_PAUSE, _mengine_stop_sound);
        adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);
    }

    //ֹͣ�ر��ļ�
    mmm_mp_cmd(mp_handle, MMM_MP_CLEAR_FILE, (unsigned int) NULL);
    fsel_set_mode(mengine_info.eg_config.repeat_mode | mengine_info.eg_config.shuffle_flag);//����ѭ��ģʽ

    return bret;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \bool _next_or_prev(switch_mode_e switch_mode)
 * \�л�������
 * \param[in]    switch_mode �л�ģʽ
 * \param[out]   none
 * \return       bool the result
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
static bool _next_or_prev(switch_mode_e switch_mode)
{
    uint8 loop_mode;
    bool bret = TRUE;

    uint8 direct = (uint8) (switch_mode & 0x01);
    uint8 force_mod = (uint8) ((switch_mode & 0x02) >> 1);

    mengine_config_t *cfg_ptr = &mengine_info.eg_config;

    mengine_info.eg_status.err_status = EG_ERR_NONE;

    loop_mode = cfg_ptr->shuffle_flag;//ѭ��ģʽshuffle

    //ǿ���л�
    if (force_mod == TRUE)
    {
        //loop_mode = cfg_ptr->shuffle_flag;//ѭ��ģʽ����Ϊȫ��ѭ��
        loop_mode |= FSEL_MODE_LOOPALL;
        fsel_set_mode(loop_mode);
    }

    if (g_change_path_flag == 1)
    {

        //�Ƿ����·������
        bret = change_locat_deal();
    }

    //����и���location���Ƿ�ɹ�
    if (bret == FALSE)
    {
        goto _switch_end;
    }

    if (direct == 0)
    {
        if ((g_del_curfile == 1) && (cfg_ptr->fsel_type <= FSEL_TYPE_DISKSEQUNCE))
        {
            //g_del_curfile = 0;
            //Ŀ¼ģʽ��Ҫ����ɨ�����
            fsel_set_typebit(file_type_cfg, FALSE);
        }

        if (g_del_curfile == 1)
        {
            cfg_ptr->location.flist_location.file_num++;
            if (cfg_ptr->location.flist_location.file_num > cfg_ptr->location.flist_location.file_total)
            {
                cfg_ptr->location.flist_location.file_num = 1;
            }
            g_del_curfile = 0;
            //��ȡ��һ��
            bret = fsel_get_byno((void*) cfg_ptr->location.plist_location.filename,
                    cfg_ptr->location.flist_location.file_num);
        }
        else
        {
            //��ȡ��һ��
            bret = fsel_get_nextfile((void*) cfg_ptr->location.plist_location.filename);
        }
    }
    else
    {
        //��ȡ��һ��
        bret = fsel_get_prevfile((void*) cfg_ptr->location.plist_location.filename);
    }

    //��ȡ��ǰ������location
    if (bret == TRUE)
    {
        fsel_get_location(&(cfg_ptr->location.plist_location), cfg_ptr->fsel_type);

        mengine_info.eg_playinfo.cur_file_switch |= 0x01;

        //��ͨ�����л�ʱ��ǰʱ��������
        mengine_info.eg_playinfo.cur_time = 0;
    }
    else
    {
        //�ָ����һ�ײ��Ÿ�����Ŀ¼��
        fsel_get_prevfile((void*) cfg_ptr->location.plist_location.filename);
    }

    _switch_end:
    //ǿ���л�
    if (force_mod == TRUE)
    {
        //loop_mode = cfg_ptr->shuffle_flag;//�ָ�ѭ��ģʽ
        loop_mode |= (uint8) cfg_ptr->repeat_mode;
        fsel_set_mode(loop_mode);
    }

    //����ϵ���Ϣ
    libc_memset(&cfg_ptr->bk_infor, 0, sizeof(mmm_mp_bp_info_t));

    return bret;

}
/******************************************************************************/
/*!
 * \par  Description:
 * \ʵ���ļ��л��������������β����һ�������˵�ͷ����һ������
 * \param[in]    stop_mode_e stop_mode ֹͣģʽ STOP_NORMAL/STOP_PAUSE
 * \param[in]    switch_mode_e force_mode �л�ģʽ
 * \li NORMAL_SWITCH_NEXT ��������һ��
 * \li NORMAL_SWITCH_PREV ��������һ��
 * \li FORCE_SWITCH_NEXT  ǿ������һ��
 * \li FORCE_SWITCH_NEXT  ǿ������һ��
 * \li END_SWITCH         ���ŵ��ļ�β����һ��
 * \param[in]    play_mode_e play_mode ����ģʽ
 * \param[out]   none
 * \return       switch_result_e
 * \retval       SWITCH_NO_ERR success
 * \retval       other failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
switch_result_e mengine_file_switch(stop_mode_e stop_mode, switch_mode_e switch_mode, play_mode_e play_mode)
{
    bool ret;
    switch_result_e switch_result = SWITCH_NO_ERR;

    //ֹͣ��ǰ��������
    ret = _stop(stop_mode);

    if (ret == FALSE)
    {
        switch_result = SWITCH_ERR_STOP;
        goto switch_end;
    }
    //��⿨���Ƿ����
    ret = _mengine_check_sd_in();

    if (ret == FALSE)
    {
        switch_result = SWITCH_ERR_STOP;
        goto switch_end;
    }

    //�л�������
    ret = _next_or_prev(switch_mode);
    if (ret == FALSE)
    {
        //�л�ʧ��ʱ����Ƿ񲥷ŵ����һ�׸���
        if (switch_mode == END_SWITCH)
        {
            if (play_mode == PLAY_FFON)
            {
                play_mode = PLAY_NO_PLAY;
            }
            else
            {
                //ǿ���л�����һ�׸�����ֹͣ���ţ�ֻ�ǻ�ȡ��Ϣ
                _next_or_prev(FORCE_SWITCH_NEXT);
                //����л���־

                play_mode = PLAY_NO_PLAY;
                g_play_mode = PLAY_NO_PLAY;
            }
        }
        else
        {
            switch_result = SWITCH_ERR_SWITCH;
            goto switch_end;
        }
    }

    //ǰ̨����music UI����ǰ̨��������״̬��̨�и貢�Զ�����
    //������ǰ̨���Ͳ���
    if ((g_app_info_state.app_state != APP_STATE_PLAYING) || (g_app_info_state.screensave_state == TRUE))
    {
        if (play_mode != PLAY_NO_PLAY)
        {
            //ǰ̨��music�����Զ��и貥�ű�־��1
            mengine_info.eg_playinfo.cur_file_switch |= 0x10;

            ret = _play(play_mode);

            if (ret == FALSE)
            {
                switch_result = SWITCH_ERR_PLAY;
            }

        }
        else
        {
            change_engine_state(ENGINE_STATE_PAUSE);
        }
    }
    else
    {
        //��ȡ�ļ���Ϣ,ǰ̨���Ͳ���
        ret = _set_file();
        if (ret == 0)
        {
            //setfile��־����,playʱ��Ҫ����setfile
            g_set_file_flag = FALSE;
        }
        //����״̬����,����Ҫ����
        g_eg_status_p->err_status = EG_ERR_NONE;
        //ǰ̨��music�����Զ��и貥�ű�־��0
        mengine_info.eg_playinfo.cur_file_switch &= (uint8)0xef;

        if (play_mode != PLAY_NO_PLAY)
        {
            g_play_mode = play_mode;
            if (g_play_mode == PLAY_FBON)//����ǿ����и�,��ʱ�丳����ǰʱ��
            {
                mengine_info.eg_playinfo.cur_time = mengine_info.eg_file_info.total_time;
            }
        }
    }
    switch_end: return switch_result;
}

