/*******************************************************************************
 *                              US212A
 *                            Module: music engine
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      fiona.yang  2011-09-05        1.0              create this file
 *******************************************************************************/

#include "app_mengine.h"
#include "audio_device.h"
#include "mengine_rdata.h"
#include "Common_func.h"


/******************************************************************************/
/*!
 * \par  Description:
 * \void _load_cfg(void)
 * \��ȡ������Ϣ
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval
 * \retval
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
void _load_cfg(void)
{

    //ϵͳ������ȡ
    sys_vm_read(&g_setting_comval, VM_AP_SETTING, sizeof(setting_comval_t));
    //sys_vm_read(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    //music engine ������ȡ
    sys_vm_read(&mengine_info.eg_config, VM_AP_MENGINE, sizeof(mengine_config_t));

#ifdef CUR_FILE_SWITCH
    sys_vm_read(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var_t));
    /*Ϊ��ȷ����һ�ο����������ֲ���ʱ
     �������ֵ�Ĭ��ֵ
     */
    g_config_var.magic = 0x55AA;
    g_config_var.ap_id = APP_ID_MUSIC;
    g_config_var.engine_state = 0x01;//(uint8) (((uint32) argc >> 16) & 0xff);
    sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var_t));
#endif

    if (mengine_info.eg_config.magic != VRAM_MAGIC(VM_AP_MENGINE))
    {
        mengine_info.eg_config.magic = VRAM_MAGIC(VM_AP_MENGINE);
        //�ļ�ѭ��ģʽ
        mengine_info.eg_config.repeat_mode = FSEL_MODE_LOOPALL;
        //shuffle flag
        mengine_info.eg_config.shuffle_flag = 0;
        //����ͨ��ģʽ��0��������1��ֻ�����������2��ֻ�����������3��������������
        mengine_info.eg_config.chanel_select_mode = 0;
        //�ļ�ѡ������ �б�/�ղؼ�/audible/Ŀ¼
        //mengine_info.eg_config.fsel_type = FSEL_TYPE_PLAYLIST;
        mengine_info.eg_config.fsel_type = FSEL_TYPE_COMMONDIR;
        //����ʱ�䵥λ����
        mengine_info.eg_config.fade_out_time = 0;
        //����ʱ�䵥λ����
        mengine_info.eg_config.fade_in_time = 0;
        //�������
        mengine_info.eg_config.fff_step = 2;
        //��ǰ�ļ����
        mengine_info.eg_config.file_cnt.num = 0;
        //��ǰ�ļ��½�����
        mengine_info.eg_config.file_cnt.total = 0;
        //(��¼��ǰ�ļ���·����Ϣ/�б��ļ��Ĳ㼶��ϵ)
        libc_memset(&mengine_info.eg_config.location.dirlocation, 0, sizeof(musfile_location_u));

        //�ϵ���Ϣ
        libc_memset(&mengine_info.eg_config.bk_infor, 0, sizeof(mmm_mp_bp_info_t));
    }

    if (mengine_enter_mode == ENTER_START)
    {
        mengine_info.eg_config.fsel_type = FSEL_TYPE_COMMONDIR;
        //(��¼��ǰ�ļ���·����Ϣ/�б��ļ��Ĳ㼶��ϵ)
        libc_memset(&mengine_info.eg_config.location.dirlocation, 0, sizeof(musfile_location_u));
        mengine_info.eg_config.location.dirlocation.disk = (uint8) g_open_disk;
    }
    else
    {
        //����ļ���Ч��
        if (mengine_info.eg_config.location.dirlocation.disk == DISK_H)
        {
            if (sys_detect_disk(DRV_GROUP_STG_CARD) == -1)
            {
                mengine_info.eg_config.fsel_type = FSEL_TYPE_COMMONDIR;
                libc_memset(&mengine_info.eg_config.location.dirlocation, 0, sizeof(musfile_location_u));
                mengine_info.eg_config.location.dirlocation.disk = DISK_C;
            }
        }

        //��Ҫ������ȡ�ļ���Ϣ
        mengine_info.eg_playinfo.cur_file_switch |= 0x01;
    }
    //mengine_info.eg_config.volume = g_comval.volume_current;

}

/******************************************************************************/
/*!
 * \par  Description:
 * \void _save_cfg(void)
 * \����������Ϣ
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval
 * \retval
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
void _save_cfg(void)
{
    /*����VM����*/
    //setting_comval_t temp_val;
    if (mengine_enter_mode != ENTER_ALARM)
    {
        //����Ӧ�ò������κα���
        sys_vm_write(&mengine_info.eg_config, VM_AP_MENGINE, sizeof(mengine_config_t));
        //sys_vm_read(&temp_val, VM_AP_SETTING, sizeof(setting_comval_t));
        //ע��ֻ����������Ҫ������ֵ���������ݾ����ı�
        //�����п���ǰ̨���µ����ݱ����渲��
        //temp_val.g_comval.volume_current = g_comval.volume_current;
        //sys_vm_write(&temp_val, VM_AP_SETTING, sizeof(setting_comval_t));
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool mengine_file_init(void)
 * \��ʼ���ļ�ѡ����,��λ���趨�ļ�or��ǰģʽ��һ���ɲ����ļ�
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
bool mengine_file_init(void)
{
    fsel_param_t init_param;//�ļ�ѡ������ʼ������
    uint8 exname[4];

    plist_location_t* loaction_ptr;
    fsel_type_e file_source;
    bool init_result = FALSE;
    //uint8 loop_cnt = 0;
    mengine_config_t *cfg_ptr = &mengine_info.eg_config;

    //��ȡ��׺������
    //get_config(CFG_MUSIC_FILE_EXT,init_param.file_type_bit);

    loaction_ptr = &(cfg_ptr->location.plist_location);

    //�ļ�ѡ������ʼ���ӿڵĲ�������

    //�̷���Ϣ
    init_param.disk = loaction_ptr->disk;

    file_source = cfg_ptr->fsel_type;

    init_param.fsel_type = file_source;

    init_param.fsel_mode = (uint8) (cfg_ptr->repeat_mode | cfg_ptr->shuffle_flag);

    init_param.file_type_bit = MUSIC_BITMAP;

    file_type_cfg = init_param.file_type_bit;

    //�ļ�ѡ������ʼ��
    file_sys_id = fsel_init(&init_param, 0);
    if (file_sys_id != -1)
    {
        //�����ϴ��˳����ļ�
        init_result = fsel_set_location(loaction_ptr, file_source);

        if (init_result == TRUE)
        {
            //�ȽϺ�׺��
            vfs_get_name(file_sys_id, exname, 0);

            if (libc_strncmp(exname, loaction_ptr->filename, sizeof(exname)) != 0)
            {
                init_result = FALSE;
            }
        }

        //����ʧ��
        if (init_result == FALSE)
        {
            //����ϵ���Ϣ
            libc_memset(&cfg_ptr->bk_infor, 0, sizeof(mmm_mp_bp_info_t));

            //��ȡ��ǰģʽ�µĵ�һ���ɲ����ļ�
            init_result = fsel_get_nextfile(loaction_ptr->filename);
            //�ҵ��ɲ����ļ�
            if (init_result == TRUE)
            {
                //��ȡ��ǰ�ļ���location��Ϣ
                init_result = fsel_get_location(loaction_ptr, file_source);
            }
            else
            {
                //û�пɲ����ļ�
                loaction_ptr->file_total = 0;
            }
        }
    }

    return init_result;

}

/******************************************************************************/
/*!
 * \par  Description:
 * \int _app_init(void)
 * \��ʼ�����桢��ȡ������Ϣ����ʼ���ļ�ѡ�������м��
 * \param[in]    void  para1
 * \param[out]   none
 * \return       int the result
 * \retval           1 sucess
 * \retval          -1 failed
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
int _app_init(void)
{
    int app_init_ret = 0;
    bool init_fsel_ret;
    char* mmm_name =
    { "mmm_mp.al" };

    //��ʼ��applib�⣬��̨AP
    applib_init(APP_ID_MENGINE, APP_TYPE_CONSOLE);

    //��Ƶ
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL6, FREQ_NULL);

    //��ȡ������Ϣ (����vm��Ϣ)
    _load_cfg();

    //��ʼ�� applib ��Ϣģ��
    applib_message_init(NULL);

    //��ʼ����ʱ��
    init_app_timers(mengine_app_timer_vector, APP_TIMER_COUNT);

    //adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL6, FREQ_NULL);

    //�����м������
    sys_load_mmm(mmm_name, TRUE);//������
    //��ʼ���ļ���ȡ�ɲ����ļ�/�ϵ�
    init_fsel_ret = mengine_file_init();

    //��ʼ���м����
    app_init_ret = mmm_mp_cmd(&mp_handle, MMM_MP_OPEN, (unsigned int) NULL);

    g_eg_cfg_p->fade_in_time = (uint32)g_setting_comval.g_comval.fade_in_time* 50;
    g_eg_cfg_p->fade_out_time = (uint32)g_setting_comval.g_comval.fade_out_time * 50;
    
    mmm_mp_cmd(mp_handle, MMM_MP_FADE_IN_TIME, g_eg_cfg_p->fade_in_time);
    mmm_mp_cmd(mp_handle, MMM_MP_FADE_OUT_TIME, g_eg_cfg_p->fade_out_time);
    mmm_mp_cmd(mp_handle, MMM_MP_GET_PFD, (uint32) & g_prefetch_data);

    if (app_init_ret == 0)
    {
        if (init_fsel_ret == TRUE)
        {
            _file_switch_info();
        }

        if (mengine_enter_mode == ENTER_START)
        {
            //ENTER_STARTģʽ��������PA,����ģʽ�ȵ����Ž���ˢ��
            //֮���ٿ���PA
            com_set_sound_out(TRUE, SOUND_OUT_START, NULL);
            g_eg_playinfo_p->cur_time = 0;
        }
        else
        {
            g_eg_playinfo_p->cur_time = (uint32) g_eg_cfg_p->bk_infor.bp_time_offset;
        }
    }

   
	
    g_delay_time = 2;
    //��һ�ν���������ѯ״̬���Ը���mengine����
    g_check_status_flag = TRUE;

    return app_init_ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool _app_deinit(void)
 * \�˳�app�Ĺ��ܺ���,����������Ϣ
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
bool _app_deinit(void)
{

    //��Ƶ
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL10, FREQ_NULL);

    //�������˳�ǰ�޸Ŀ�״̬
    if (mengine_info.eg_config.location.plist_location.disk == DISK_H)
    {
        if (sys_detect_disk(DRV_GROUP_STG_CARD) != -1)
        {
            //���ڿ���
            //��ǰ�̷��ǿ���,����״̬Ϊ��δ����״̬
            change_card_state(CARD_STATE_CARD_IN);
        }
        else
        {
            change_card_state(CARD_STATE_CARD_NULL);
        }

    }

    //�ر��ļ�ѡ����
    fsel_exit();

    //�ر������豸
    //mmm_mp_cmd(mp_handle, MMM_MP_AOUT_CLOSE, (unsigned int) NULL);
    //�ر��м���豸
    mmm_mp_cmd(mp_handle, MMM_MP_CLOSE, (unsigned int) NULL);

    //disable_dac();
    //disable_pa();
    com_set_sound_out(FALSE, SOUND_OUT_STOP, NULL);

    //ִ��applib���ע������
    applib_quit();

    //save config дvram
    _save_cfg();

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);
    //ж�ؽ�������
    sys_free_mmm(TRUE);

    //��Ƶ
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \int main(int argc, const char *argv[])
 * \app��ں���
 * \param[in]    argc  para1
 * \param[in]    argc  para2
 * \param[out]   none
 * \return       int the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
int main(int argc, const char *argv[])
{
    
    mengine_enter_mode = (mengine_enter_mode_e) (argc & 0xc0);

    //��alarm���룬�������޸�ΪENTER_START
    //alarm�������Ų���Ҫ֧�ֶϵ㲥�ţ�����Ҫ����location
    if (mengine_enter_mode == ENTER_ALARM)
    {
        mengine_enter_mode = ENTER_START;
    }

    //����DAE���ò����������ڴ�
    g_dae_cfg_shm = (dae_config_t *) sys_shm_mount(SHARE_MEM_ID_DAECFG);
    if (g_dae_cfg_shm == NULL)
    {
        PRINT_ERR("dae_cfg shm not exist!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }
    g_open_disk = (uint8) (argc & 0x3f);

    g_eg_cfg_p = &mengine_info.eg_config;

    g_eg_status_p = &mengine_info.eg_status;

    g_eg_playinfo_p = &mengine_info.eg_playinfo;
    //music_print("INT_ENGINE",mengine_enter_mode,2);
    //��ʼ��
    //if (_app_init() == 1)
    _app_init();
    //{
    g_mengine_result = mengine_control_block();//app���ܴ���z
    //}
    //���»ָ�enter_mode
    mengine_enter_mode = (mengine_enter_mode_e) (argc & 0xc0);
    _app_deinit();//�˳�
    //music_print("OUT_ENGINE",0,0);
    return g_mengine_result;
}
