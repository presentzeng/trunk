/*******************************************************************************
 *                              US212A
 *                            Module: music engine
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      yanghong  2015-12-25        1.0              create this file
 *******************************************************************************/

#include "app_mengine.h"
#include "mengine_rdata.h"

//歌曲播放系数表
const uint8 asrc_coeffi_file[] = "coeffi.bin";
/******************************************************************************/
/*!
 * \par  Description:
 * \bool _mengine_start_sound(uint8 cur_volume)
 * \开始播放时打开声音输出回调函数
 * \param[in]    cur_volume
 * \param[out]   none
 * \return       bool
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
bool _mengine_start_sound(uint8 cur_volume)
{
    //param1 audio codec, param2 sample rate
    mengine_info.eg_config.volume = cur_volume;
    //    enable_dac(2, 44);
    com_set_phy_volume(cur_volume);
    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool _mengine_stop_sound(uint8 cur_volume)
 * \停止播放时打开声音输出回调函数
 * \param[in]    cur_volume
 * \param[out]   none
 * \return       bool
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
bool _mengine_stop_sound(void)
{
    disable_dac(0);//需要进一步调试通道
    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \int _mengine_check_sd_in(void)
 * \如果为卡盘播放文件，检测卡是否存在
 * \param[in]    none
 * \param[out]   none
 * \return       bool
 * \retval       1 存在
 * \retval       0 不存在
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
bool _mengine_check_sd_in(void)
{
    bool bret = TRUE;
    if (g_eg_cfg_p->location.dirlocation.disk == DISK_H)
    {
        if (sys_detect_disk(DRV_GROUP_STG_CARD) == -1)
        {
            bret = FALSE;
        }
    }
    return bret;
}
/******************************************************************************/
/*!
* \par  Description:
* \static int8 _get_coeffi_by_sampling_rate(uint8 sample_level)
* \根据采样率获取对应的系数表
* \param[in]    int sample_rate
*                       采样率
* \param[out]   none
* \return       0:success    -1:fail
* \note
*/
/*******************************************************************************/
static int8 _get_coeffi_by_sampling_rate(int sample_rate)
{
    uint8 asrc_coeffi_name[12];
    sd_handle sd_file_handle;
    uint8 sample_level; 
    
    libc_memcpy(asrc_coeffi_name, asrc_coeffi_file, 12);
    sd_file_handle = sys_sd_fopen(asrc_coeffi_name);
    if (sd_file_handle == NULL)
    {
        return -1;
    }
    
    if (sample_rate <= 48)
    {
        sample_level = 0;
    }
    else if (sample_rate <= 96)
    {
        sample_level = 1;
    }
    else
    {
        sample_level = 2;
    }

    *((REG32) (CMU_MEMCLKSEL)) &= (~((1 << CMU_MEMCLKSEL_ASRCBUF0CLKSEL) | (1 << CMU_MEMCLKSEL_ASRCBUF1CLKSEL)));
    sys_sd_fseek(sd_file_handle, SEEK_SET, sample_level * COEFFI_OFFSET); 
    sys_sd_fread(sd_file_handle, ASRC_BUF0_ADDR, ASRC_BUF0_LEN);
    sys_sd_fread(sd_file_handle, ASRC_BUF1_ADDR, ASRC_BUF1_LEN); 
    sys_sd_fclose(sd_file_handle);
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \int _get_file_info(uint8 normal_file)
 * \设置文件并获取文件信息
 * \param[in]    normal_file 是普通music文件还是SD区文件(支持闹铃音乐播放)
 * \param[out]   none
 * \return       int 命令返回值
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_play_deal.c
 * \note
 */
/*******************************************************************************/
int _get_file_info(uint8 normal_file)
{
    int result;
    int last_sample_rate = mengine_info.eg_file_info.sample_rate;
    
    mmm_mp_fs_para_t set_file_param;
    storage_io_t file_io;

    if (normal_file == TRUE)
    {
        //normal file
        set_file_param.fs_type = 0;
        set_file_param.file_name = NULL;

        file_io.read = mmm_mp_read;
        file_io.seek = mmm_mp_seek;
        file_io.tell = mmm_mp_tell;
        //参数设置
#ifdef PC
        //mengine_config_t *eg_cfg = &mengine_info.eg_config;
        set_file_param.file_name = mengine_info.eg_config.location.plist_location.filename;
#endif
    }
    else
    {
        //sd file
        set_file_param.fs_type = 1;
        set_file_param.file_name = (char*) g_file_name;
#if 1
        file_io.read = mmm_mp_read_sd;
        file_io.seek = mmm_mp_seek_sd;
        file_io.tell = mmm_mp_tell_sd;
#endif
    }

    //盘符
    file_io.read = &mmm_mp_read;
    set_file_param.file_mount_id = (void*) file_sys_id;

    if (g_music_fileopen_flag == 1)
    {
        mmm_mp_cmd(mp_handle, MMM_MP_CLEAR_FILE, (unsigned int) NULL);
        g_music_fileopen_flag = 0;
    }
    result = mmm_mp_cmd(mp_handle, MMM_MP_SET_FILE, (unsigned int) &set_file_param);
    mmm_mp_cmd(mp_handle, MMM_MP_SET_FILE_IO, (unsigned int) &file_io);

    if (result == 0)
    {
        g_music_fileopen_flag = 1;
        //获取文件格式、总时间、比特率等信息
        result = mmm_mp_cmd(mp_handle, MMM_MP_MEDIA_INFO, (unsigned int) &mengine_info.eg_file_info);

        if (last_sample_rate != mengine_info.eg_file_info.sample_rate)
        {
            _get_coeffi_by_sampling_rate(mengine_info.eg_file_info.sample_rate);
        }
    }

    return result;
}

