/*******************************************************************************
 *                              US212A
 *                            Module: music engine
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      fiona.yang  2012-05-30        1.0              create this file
 *******************************************************************************/

#include "app_mengine.h"
#include "mengine_rdata.h"

const uint16 eq_cutoffs[MAX_GEQ_SEG] = {80,200,480,980,4000,8000,16000};
const uint16 eq_Q_values[MAX_GEQ_SEG] = {10,6,4,6,10,5,8};
//ǰ7��������Ӧ7��Ƶ�㣬�ֱ��� 80Hz��200Hz��500Hz��1kHz��4kHz��8kHz��16kHz��
const int8 eq_param[EQ_PARAM_MAX][MAX_GEQ_SEG] =
{
    { 0, 0, 0,  0,  0,  0,  0 }, //none
    { 5, 3, 0, -1,  0,  4,  5 }, //rock
    { 3, 0, 0,  0,  0, -1, -2 }, //funk--pop
    { 0, 0, 1,  2,  0,  1,  1 }, //hiphop--soft
    { 3, 0, 0, -1,  0,  2,  4 }, //Jazz
    { 0, 0, 0, -1, -1, -2, -4 }, //Classic
    { 5, 2, 0,  0,  0,  0,  6 }, //techno--dbb
};

/******************************************************************************/
/*!
 * \par  Description:
 * \void mengine_set_real_eq(uint8 mode)
 * \������м����������EQ����
 * \param[in]    mode TRUE �����и�����EQ FALSE ͨ���˵�����EQ
 * \param[out]   none
 * \return       none
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
void mengine_set_real_eq(uint8 mode)
{
    uint8 i;
    uint8 eq_type;
    uint8 eq_mode;
    dae_config_t *p_dae_cfg = &(g_setting_comval.g_comval.dae_cfg);
    
    eq_mode = mengine_info.eg_config.eq_info.eq_mode;
    eq_type = mengine_info.eg_config.eq_info.eq_type;
    g_setting_comval.g_comval.eq_type = eq_mode;
    
    //��ӡEQ����
    if(p_dae_cfg->peq_enable == 0)
    {
        return;
    }
    
    p_dae_cfg->bypass = FALSE;
    for (i = 0; i < MAX_GEQ_SEG; i++)
    {
        p_dae_cfg->peq_bands[i].cutoff = eq_cutoffs[i];
        p_dae_cfg->peq_bands[i].q = eq_Q_values[i];
        p_dae_cfg->peq_bands[i].gain = 10 * eq_param[eq_type][i];
        p_dae_cfg->peq_bands[i].type = 1;
    }

    for(; i < MAX_EQ_SEG; i++)
    {
        p_dae_cfg->peq_bands[i].gain = 0;
    }
    
    //����DAE���ò����������ڴ�
    libc_memcpy(g_dae_cfg_shm, p_dae_cfg, sizeof(dae_config_t));
    g_dae_cfg_shm = (dae_config_t *) sys_shm_mount(SHARE_MEM_ID_DAECFG);
    if (g_dae_cfg_shm == NULL)
    {
        PRINT_ERR("dae_cfg shm not exist!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }
    
    //���õ�ǰeq��������
    mmm_mp_cmd(mp_handle, MMM_MP_SET_EQ, (uint32)NULL);

    if (mode == TRUE)
    {
        //��������
        com_set_sound_volume(get_current_volume());
    }

    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_set_eq(void* msg_ptr)
 * \����eq
 * \param[in]    msg_ptr��Ϣָ��  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//ͬ����־��1B
 *                 sem_id_t sem_id; 		  //�ź���ID��1B
 *                 msg_apps_t msg;		    //��Ϣ���ݣ�6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//��Ϣ���ͣ�2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//��Ϣ���ݻ����ַ������Ϣ�Լ�Լ����4B
 *               } msg_apps_t;//˽����Ϣ���е���Ϣ�ṹ��

 * \param[out]   msg_ptr->msg.content.addr //����eq��Ϣ��ָ��
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_set_eq(void* msg_ptr)
{

    //����eq�������ݵ�ָ��
    private_msg_t * data_ptr = (private_msg_t*) msg_ptr;

    //����Ϣָ��ָ�������copy���������
    libc_memcpy(&(g_eg_cfg_p->eq_info), (data_ptr->msg.content.addr), sizeof(eq_param_t));

    //���õ�ǰ��EQ
    mengine_set_real_eq(0);

    //���سɹ�
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}


/******************************************************************************/
/*!
 * \par  Description:
 * \�ϵ���������
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval       1 success
 * \retval       0 failed
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_standby_play(void* msg_ptr)
{
    bool ret_vals = TRUE;
    
    if (mengine_enter_mode == ENTER_RESUME)
    {
        //������Ƶ����    
        com_set_sound_out(TRUE, SOUND_OUT_START, NULL);
        
        //�ָ����ֲ���,����
        ret_vals = _play(PLAY_RESUME);
    }
    else if(mengine_enter_mode == ENTER_RESUME_PAUSE)
    {
        //������Ƶ����
        com_set_sound_out(TRUE, SOUND_OUT_START, NULL);
        
        //�ر���Ƶ���
        com_set_sound_out(FALSE, SOUND_OUT_PAUSE, NULL);
    }
    else
    {
        ;//nothing for QAC
    }

    //���سɹ�
    mengine_reply_msg(msg_ptr, ret_vals); 

    return RESULT_IGNORE;

}


