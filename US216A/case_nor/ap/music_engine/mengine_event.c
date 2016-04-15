/*******************************************************************************
 *                              US212A
 *                            Module: music engine
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      fiona.yang  2011-09-07        1.0              create this file
 *******************************************************************************/

#include "app_mengine.h"
#include "mengine_rdata.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_config(void* msg_ptr)
 * \��ȡ���浱ǰ��������Ϣ
 * \param[in]    msg_ptr��Ϣָ��  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//ͬ����־��1B
 *                 sem_id_t sem_id; 		  //�ź���ID��1B
 *                 msg_reply_t *reply;    //ͬ����Ϣ��ִָ��
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

 * \param[out]   msg_ptr->msg.content.addr //�������������Ϣ
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
/*app_result_e mengine_get_config(void* msg_ptr)
 {
 //��Ϣָ��
 private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

 //copy������Ϣ����Ϣָ��ָ���������
 libc_memcpy((data_ptr->reply->content), &mengine_info.eg_config, sizeof(mengine_config_t));

 //���سɹ�
 data_ptr->reply->type = MSG_REPLY_SUCCESS;

 //�ظ�ͬ����Ϣ(�����ź���)
 reply_sync_msg(msg_ptr);

 return RESULT_IGNORE;
 }*/

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_filepath(void* msg_ptr)
 * \��ȡ��ǰ�����ļ���location��Ϣ
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

 * \param[out]   msg_ptr->msg.content.addr //����ļ�·����Ϣ
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_filepath(void* msg_ptr)
{
    uint8 location_size;
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //eg_config
    mengine_config_t* egcfg_ptr = (mengine_config_t*) &(mengine_info.eg_config);

    //����·����ָ��
    file_path_info_t* locat_info = (file_path_info_t*) (data_ptr->reply->content);

    //��ǰ�ļ�����
    fsel_type_e file_source = egcfg_ptr->fsel_type;

    if ((file_source == FSEL_TYPE_PLAYLIST) || (file_source == FSEL_TYPE_LISTAUDIBLE) || (file_source
            == FSEL_TYPE_M3ULIST))
    {
        //�ǲ����б�ģʽ
        location_size = sizeof(plist_location_t);
    }
    else if ((file_source == FSEL_TYPE_COMMONDIR) || (file_source == FSEL_TYPE_DISKSEQUNCE))
    {
        //��Ŀ¼ģʽ
        location_size = sizeof(file_location_t);
    }
    else
    {
        //���ղؼ�ģʽ
        location_size = sizeof(flist_location_t);
    }
    libc_memcpy(&(locat_info->file_path), &(egcfg_ptr->location), (int) location_size);

    locat_info->file_source = egcfg_ptr->fsel_type;//��ǰ�ļ�ģʽ

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_fileinfo(void* msg_ptr)
 * \��ȡ��ǰ���ŵ��ļ���ʽ����ʱ�䡢�����ʵ���Ϣ
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

 * \param[out]   msg_ptr->msg.content.addr //����ļ���Ϣ
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_fileinfo(void* msg_ptr)
{
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy��ǰ�ļ���Ϣ����Ϣָ��ָ���������
    libc_memcpy((data_ptr->reply->content), &mengine_info.eg_file_info, sizeof(mmm_mp_file_info_t));

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_status(void* msg_ptr)
 * \��ȡ��ǰ����״̬
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

 * \param[out]   msg_ptr->msg.content.addr //���״̬��Ϣ
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_status(void* msg_ptr)
{
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy��ǰ����״̬����Ϣָ��ָ���������
    libc_memcpy(data_ptr->reply->content, &mengine_info.eg_status, sizeof(mengine_status_t));

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_playmode(void* msg_ptr)
 * \��ȡѭ��ģʽ
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

 * \param[out]   msg_ptr->msg.content.addr //����ѭ����ʽ��ָ��
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_playmode(void* msg_ptr)
{
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    //����shuffleģʽ+ѭ��ģʽ
    *(uint8*) (data_ptr->reply->content) = (uint8) (mengine_info.eg_config.shuffle_flag
            | mengine_info.eg_config.repeat_mode);

    //���سɹ�
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_playinfo(void* msg_ptr)
 * \��ȡ��ǰ����ʱ��ͱ�����
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

 * \param[out]   msg_ptr->msg.content.addr //��Ų���ʱ��ͱ�����
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_playinfo(void* msg_ptr)
{
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy��ǰ����ʱ��ͱ����ʵ���Ϣָ��ָ���������
    libc_memcpy(data_ptr->reply->content, &mengine_info.eg_playinfo, sizeof(mengine_playinfo_t));

    //�����л���ʱ����б���
#ifdef CUR_FILE_SWITCH
    if ((mengine_info.eg_playinfo.cur_file_switch & 0x01) == 0x01)
    {
        sys_vm_write(&mengine_info.eg_config, VM_AP_MENGINE, sizeof(mengine_config_t));
    }
#endif

    //����ļ��л���־
    mengine_info.eg_playinfo.cur_file_switch &= (uint8) (0xfe);

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_fast_forward(void* msg_ptr)
 * \���
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

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_fast_forward(void* msg_ptr)
{
    //��ǰ����״̬
    mengine_status_t *eg_status = &mengine_info.eg_status;
    //��ǰ����AB����
    //if (eg_status->ab_status == PlayAB_Null)
    //{
    //���������� or ���
    if ((eg_status->play_status == PlaySta) || (eg_status->play_status == PlayFast))
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 4);//���
        eg_status->play_status = PlayFast;//�޸�״̬
        eg_status->fast_status = FFPlaySta;
    }
    //}
    //���سɹ�
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_fast_backward(void* msg_ptr)
 * \����
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

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_fast_backward(void* msg_ptr)
{
    mengine_status_t *eg_status = &mengine_info.eg_status;
    //��ǰ����AB����
    //if (eg_status->ab_status == PlayAB_Null)
    //{
    //��ǰ���������� or ����
    if ((eg_status->play_status == PlaySta) || (eg_status->play_status == PlayFast))
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) -4);//����
        eg_status->play_status = PlayFast;//�޸�״̬
        eg_status->fast_status = FBPlaySta;
    }
    //}

    //���سɹ�
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_cancel_ffb(void* msg_ptr)
 * \ȡ�������
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

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_cancel_ffb(void* msg_ptr)
{
    mengine_status_t *eg_status = &mengine_info.eg_status;
    //��ǰ�ǿ����״̬
    if ((eg_status->play_status == PlayFast) && (eg_status->fast_status != FBPlay_Null))
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 0);//ȡ�������
        eg_status->play_status = PlaySta;//�޸�״̬
        eg_status->fast_status = FBPlay_Null;
    }

    //���سɹ�
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_set_filepath(void* msg_ptr)
 * \���ò����ļ�·����Ϣ
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

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_filepath_deal(void* msg_ptr)
{
    uint8 location_size;
    //void *data_ptr = msg_ptr->msg.content.addr;//��Ϣ����ָ��
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    file_path_info_t *locat_info = (file_path_info_t*) data_ptr->msg.content.addr;//·����Ϣָ��

    fsel_type_e file_source = locat_info->file_source;

    if ((file_source == FSEL_TYPE_PLAYLIST) || (file_source == FSEL_TYPE_LISTAUDIBLE) || (file_source
            == FSEL_TYPE_M3ULIST))
    {
        //�ǲ����б�ģʽ
        location_size = sizeof(plist_location_t);
    }
    else if ((file_source == FSEL_TYPE_COMMONDIR) || (file_source == FSEL_TYPE_DISKSEQUNCE))
    {
        //��Ŀ¼ģʽ
        location_size = sizeof(file_location_t);
    }
    else
    {
        //���ղؼ�ģʽ
        location_size = sizeof(flist_location_t);
    }
    libc_memcpy(&(mengine_info.eg_config.location), &(locat_info->file_path), (int) location_size);

    //mengine_info.eg_config.fsel_type = cur_type;
    /*_set_file();//���ò����ļ�*/
    return RESULT_IGNORE;

}

/******************************************************************************/
/*!
 * \par  Description:
 * \uinapp_result_e mengine_set_filepath(void* msg_ptr)
 * \���ò����ļ�·����Ϣ
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

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_set_filepath(void* msg_ptr)
{
    bool ret_vals;
    //uint8 loop_cnt;
    bool need_fsel_init = FALSE;

    //�̷�
    uint8 disk_new, disk_old;
    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    file_path_info_t *locat_info = (file_path_info_t*) data_ptr->msg.content.addr;//·����Ϣָ��

    fsel_type_e cur_type = locat_info->file_source;
    plist_type_e cur_plist_type = mengine_info.eg_config.location.plist_location.list_type;

    if (locat_info->file_source == FSEL_TYPE_SDFILE)
    {
        ret_vals = _stop(STOP_NORMAL);//ֹͣ����
        mengine_info.eg_config.fsel_type = locat_info->file_source;//�������汣����ļ�ѡ������
        libc_memcpy(g_file_name, locat_info->file_path.plist_location.filename, 12);
    }
    else
    {
        //ע��_stop��������g_is_audible,���Ҫ��ֹͣ��������
        ret_vals = _stop(STOP_NORMAL);//ֹͣ����
        if (ret_vals == FALSE)
        {
            goto msg_end;
        }

        //�ļ�ѡ�������޸ģ���Ҫ���³�ʼ���ļ�ѡ����(�̷��޸�Ҳ��Ҫ���³�ʼ��)
        disk_new = locat_info->file_path.plist_location.disk;
        disk_old = mengine_info.eg_config.location.plist_location.disk;

        mengine_filepath_deal(msg_ptr);

        if ((mengine_info.eg_config.fsel_type != cur_type) || (disk_new != disk_old)
                || (mengine_info.eg_config.location.plist_location.list_type != cur_plist_type))
        {
            need_fsel_init = TRUE;
        }

        if (need_fsel_init == TRUE)
        {
            //�˳��ļ�ѡ����
            fsel_exit();
            mengine_info.eg_config.fsel_type = cur_type;//�������汣����ļ�ѡ������
            ret_vals = mengine_file_init();//���³�ʼ��
            if (ret_vals == FALSE)
            {
                goto msg_end;
            }
        }
        //else
        {
            ret_vals = fsel_set_location(&mengine_info.eg_config.location.plist_location, cur_type);
            if (ret_vals == FALSE)
            {
                fsel_exit();
                ret_vals = mengine_file_init();//���³�ʼ��
                if (ret_vals == FALSE)
                {
                    goto msg_end;
                }
            }
        }
        if ((mengine_info.eg_config.shuffle_flag != 0) && (file_sys_id != -1))
        {
            ret_vals = fsel_get_location(&(mengine_info.eg_config.location.plist_location),
                    mengine_info.eg_config.fsel_type);
        }

    }
    msg_end: mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_change_filepath(void* msg_ptr)
 * \�ı䲥���ļ�·��
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

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE ����
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_change_filepath(void* msg_ptr)
{

    //��Ϣָ��
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    file_path_info_t *locat_info = (file_path_info_t*) data_ptr->msg.content.addr;//·����Ϣָ��

    if (g_change_path_flag == 0)
    {
        g_path_type = mengine_info.eg_config.fsel_type;
    }
    mengine_filepath_deal(msg_ptr);

    mengine_info.eg_config.fsel_type = locat_info->file_source;
    //���¿�ʼ����, �����ļ�
    /*if (_set_file() == TRUE)
     {
     _play(PLAY_RESUME);//����
     }*/
    g_change_path_flag = 1;
    //���سɹ�
    mengine_reply_msg(msg_ptr, TRUE);
    return RESULT_IGNORE;
}

