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
 * \切换下一曲的文件信息获取
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
    
    //获取文件信息，如播放总时间
    ret = _set_file();
    if (ret == 0)
    {
        g_set_file_flag = FALSE;
    }
    //通过SET_FILE会检测命令是否正确，但对FILE_SWITCH_INFO,不关心错误号
    //需要清除错误号
    //if(g_eg_status_p->err_status != EG_ERR_NONE)
    //{
    g_eg_status_p->err_status = EG_ERR_NONE;
    //}
    
    if (mengine_enter_mode == ENTER_START)
    {
        //关闭文件
        mmm_mp_cmd(mp_handle, MMM_MP_CLEAR_FILE, (unsigned int) NULL);
    }

    return ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool _set_file(void)
 * \设置播放文件到解码器
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

    //set_file函数被调用标志置位
    g_set_file_flag = TRUE;

    //首先检测卡文件是否存在
    if (_mengine_check_sd_in() == FALSE)
    {
        return FALSE;
    }

    //文件系统挂载id
    if (g_eg_cfg_p->fsel_type == FSEL_TYPE_SDFILE)
    {
        result = _get_file_info(FALSE);
    }
    else
    {
        //先清除歌词标志
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
 * \开始播放音乐
 * \param[in]    play_mode 播放模式 para1
 * \param[in]    play_mode_e:
 *							 \PLAY_NORMAL 正常开始播放
 *							 \PLAY_RESUME 断点续播
 *							 \PLAY_FFON   快进播放
 *							 \PLAY_FBON   快退播放
 * \param[out]   none
 * \return       bool the result
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 * \li   注意如果文件SET_FILE失败，这里并不直接关闭句柄,而是先通过状态检测获取错误
 号，然后再关闭句柄。如果直接调用CLEAR_FILE命令，会清除错误号
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
    //停止 or 暂停时 发送播放命令
    if ((mengine_info.eg_status.play_status == StopSta) || (mengine_info.eg_status.play_status == PauseSta))
    {
        if (g_set_file_flag == FALSE)
        {
            bret = _set_file();
        }
        //PLAY后需要清零标志位
        g_set_file_flag = FALSE;
        
        if (bret == FALSE)
        {
            goto end_play;
        }
        com_set_sound_out(TRUE, SOUND_OUT_RESUME, _mengine_start_sound);

        //有可能从不支持播放的类型切换到支持播放的类型的EQ,因此需要设置
        mengine_set_real_eq(TRUE);

        if (play_mode == PLAY_FFON)
        {
            //快进播放
            result = mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 4);
            mengine_info.eg_status.play_status = PlayFast;//设置播放状态
        }
        else if (play_mode == PLAY_FBON)
        {
            //快退播放
            mengine_info.eg_config.bk_infor.bp_time_offset = -5000;
            mengine_info.eg_config.bk_infor.bp_file_offset = 0;
            mengine_info.eg_config.bk_infor.bp_info_ext = 0;
            mmm_mp_cmd(mp_handle, MMM_MP_SET_BREAK_POINT, (uint32) (&(mengine_info.eg_config.bk_infor)));

            result = mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) -4);
            mengine_info.eg_status.play_status = PlayFast;//设置播放状态
        }
        else if (play_mode == PLAY_RESUME)
        {
            //断点续播,设置断点
            mmm_mp_cmd(mp_handle, MMM_MP_SET_BREAK_POINT, (uint32) (&(mengine_info.eg_config.bk_infor)));
            mengine_info.eg_status.play_status = PlaySta;//设置播放状态
        }
        else
        {
            mengine_info.eg_status.play_status = PlaySta;//设置播放状态
        }

        //发送播放命令
        result = mmm_mp_cmd(mp_handle, MMM_MP_PLAY, (uint32) NULL);
        if (result != 0)
        {
            bret = FALSE;
        }
        else
        {
            if (libc_memcmp(mengine_info.eg_config.location.dirlocation.filename, "AAC", 4) == 0)
            {
                //获取文件格式、总时间、比特率等信息
                result = mmm_mp_cmd(mp_handle, MMM_MP_MEDIA_INFO, (uint32) & mengine_info.eg_file_info);
            }
            bret = TRUE;
        }
        //浏览播放,设置计时起始时间
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
 * \停止播放音乐
 * \param[in]    stop_mode 停止模式 para1
 * \param[in]    stop_mode_e:
 * 							 \STOP_NORMAL 正常停止
 * 							 \STOP_PAUSE  暂停播放
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

    //取消快进退 回到正常播放状态
    if (eg_status->play_status == PlayFast)
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 0);
        eg_status->play_status = PlaySta;
    }
    //正常播放才会停止
    if (eg_status->play_status == PlaySta)
    {
        //保存断点
        mmm_mp_cmd(mp_handle, MMM_MP_GET_BREAK_POINT, (unsigned int) &(mengine_info.eg_config.bk_infor));
        //停止播放
        result = mmm_mp_cmd(mp_handle, MMM_MP_STOP, (unsigned int) NULL);
        if (result != 0)
        {
            bret = FALSE;
        }
        else
        {
            //暂停播放
            if (stop_mode == STOP_PAUSE)
            {
                eg_status->play_status = PauseSta;
            }
            else
            {
                //正常停止
                eg_status->play_status = StopSta;
                mengine_info.eg_config.file_cnt.num = 1;
            }
            bret = TRUE;
        }

        if (bret == FALSE)
        {
            mengine_save_errno_no();
        }

        //关闭音频输出
        com_set_sound_out(FALSE, SOUND_OUT_PAUSE, _mengine_stop_sound);
        adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);
    }

    //停止关闭文件
    mmm_mp_cmd(mp_handle, MMM_MP_CLEAR_FILE, (unsigned int) NULL);
    fsel_set_mode(mengine_info.eg_config.repeat_mode | mengine_info.eg_config.shuffle_flag);//设置循环模式

    return bret;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \bool _next_or_prev(switch_mode_e switch_mode)
 * \切换上下曲
 * \param[in]    switch_mode 切换模式
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

    loop_mode = cfg_ptr->shuffle_flag;//循环模式shuffle

    //强制切换
    if (force_mod == TRUE)
    {
        //loop_mode = cfg_ptr->shuffle_flag;//循环模式设置为全部循环
        loop_mode |= FSEL_MODE_LOOPALL;
        fsel_set_mode(loop_mode);
    }

    if (g_change_path_flag == 1)
    {

        //是否更跟路径？？
        bret = change_locat_deal();
    }

    //如果有更改location，是否成功
    if (bret == FALSE)
    {
        goto _switch_end;
    }

    if (direct == 0)
    {
        if ((g_del_curfile == 1) && (cfg_ptr->fsel_type <= FSEL_TYPE_DISKSEQUNCE))
        {
            //g_del_curfile = 0;
            //目录模式需要重新扫描磁盘
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
            //获取下一曲
            bret = fsel_get_byno((void*) cfg_ptr->location.plist_location.filename,
                    cfg_ptr->location.flist_location.file_num);
        }
        else
        {
            //获取下一曲
            bret = fsel_get_nextfile((void*) cfg_ptr->location.plist_location.filename);
        }
    }
    else
    {
        //获取上一曲
        bret = fsel_get_prevfile((void*) cfg_ptr->location.plist_location.filename);
    }

    //获取当前歌曲的location
    if (bret == TRUE)
    {
        fsel_get_location(&(cfg_ptr->location.plist_location), cfg_ptr->fsel_type);

        mengine_info.eg_playinfo.cur_file_switch |= 0x01;

        //普通歌曲切换时当前时间先清零
        mengine_info.eg_playinfo.cur_time = 0;
    }
    else
    {
        //恢复最后一首播放歌曲的目录项
        fsel_get_prevfile((void*) cfg_ptr->location.plist_location.filename);
    }

    _switch_end:
    //强制切换
    if (force_mod == TRUE)
    {
        //loop_mode = cfg_ptr->shuffle_flag;//恢复循环模式
        loop_mode |= (uint8) cfg_ptr->repeat_mode;
        fsel_set_mode(loop_mode);
    }

    //清除断点信息
    libc_memset(&cfg_ptr->bk_infor, 0, sizeof(mmm_mp_bp_info_t));

    return bret;

}
/******************************************************************************/
/*!
 * \par  Description:
 * \实现文件切换上下曲，快进到尾切下一曲，快退到头切上一曲功能
 * \param[in]    stop_mode_e stop_mode 停止模式 STOP_NORMAL/STOP_PAUSE
 * \param[in]    switch_mode_e force_mode 切换模式
 * \li NORMAL_SWITCH_NEXT 正常切下一曲
 * \li NORMAL_SWITCH_PREV 正常切上一曲
 * \li FORCE_SWITCH_NEXT  强制切下一曲
 * \li FORCE_SWITCH_NEXT  强制切上一曲
 * \li END_SWITCH         播放到文件尾切下一曲
 * \param[in]    play_mode_e play_mode 播放模式
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

    //停止当前歌曲播放
    ret = _stop(stop_mode);

    if (ret == FALSE)
    {
        switch_result = SWITCH_ERR_STOP;
        goto switch_end;
    }
    //检测卡盘是否存在
    ret = _mengine_check_sd_in();

    if (ret == FALSE)
    {
        switch_result = SWITCH_ERR_STOP;
        goto switch_end;
    }

    //切换上下曲
    ret = _next_or_prev(switch_mode);
    if (ret == FALSE)
    {
        //切换失败时检测是否播放到最后一首歌曲
        if (switch_mode == END_SWITCH)
        {
            if (play_mode == PLAY_FFON)
            {
                play_mode = PLAY_NO_PLAY;
            }
            else
            {
                //强制切换到下一首歌曲并停止播放，只是获取信息
                _next_or_prev(FORCE_SWITCH_NEXT);
                //清除切换标志

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

    //前台不是music UI或者前台进入屏保状态后台切歌并自动播放
    //否则由前台发送播放
    if ((g_app_info_state.app_state != APP_STATE_PLAYING) || (g_app_info_state.screensave_state == TRUE))
    {
        if (play_mode != PLAY_NO_PLAY)
        {
            //前台非music进行自动切歌播放标志置1
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
        //获取文件信息,前台发送播放
        ret = _set_file();
        if (ret == 0)
        {
            //setfile标志清零,play时需要重新setfile
            g_set_file_flag = FALSE;
        }
        //错误状态清零,不需要关心
        g_eg_status_p->err_status = EG_ERR_NONE;
        //前台是music进行自动切歌播放标志清0
        mengine_info.eg_playinfo.cur_file_switch &= (uint8)0xef;

        if (play_mode != PLAY_NO_PLAY)
        {
            g_play_mode = play_mode;
            if (g_play_mode == PLAY_FBON)//如果是快退切歌,总时间赋给当前时间
            {
                mengine_info.eg_playinfo.cur_time = mengine_info.eg_file_info.total_time;
            }
        }
    }
    switch_end: return switch_result;
}

