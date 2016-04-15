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
 * \获取引擎当前的配置信息
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_reply_t *reply;    //同步消息回执指针
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   msg_ptr->msg.content.addr //存放引擎配置信息
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
/*app_result_e mengine_get_config(void* msg_ptr)
 {
 //消息指针
 private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

 //copy引擎信息到消息指针指向的数据区
 libc_memcpy((data_ptr->reply->content), &mengine_info.eg_config, sizeof(mengine_config_t));

 //返回成功
 data_ptr->reply->type = MSG_REPLY_SUCCESS;

 //回复同步消息(发送信号量)
 reply_sync_msg(msg_ptr);

 return RESULT_IGNORE;
 }*/

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_filepath(void* msg_ptr)
 * \获取当前播放文件的location信息
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   msg_ptr->msg.content.addr //存放文件路径信息
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_filepath(void* msg_ptr)
{
    uint8 location_size;
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //eg_config
    mengine_config_t* egcfg_ptr = (mengine_config_t*) &(mengine_info.eg_config);

    //返回路径的指针
    file_path_info_t* locat_info = (file_path_info_t*) (data_ptr->reply->content);

    //当前文件类型
    fsel_type_e file_source = egcfg_ptr->fsel_type;

    if ((file_source == FSEL_TYPE_PLAYLIST) || (file_source == FSEL_TYPE_LISTAUDIBLE) || (file_source
            == FSEL_TYPE_M3ULIST))
    {
        //是播放列表模式
        location_size = sizeof(plist_location_t);
    }
    else if ((file_source == FSEL_TYPE_COMMONDIR) || (file_source == FSEL_TYPE_DISKSEQUNCE))
    {
        //是目录模式
        location_size = sizeof(file_location_t);
    }
    else
    {
        //是收藏夹模式
        location_size = sizeof(flist_location_t);
    }
    libc_memcpy(&(locat_info->file_path), &(egcfg_ptr->location), (int) location_size);

    locat_info->file_source = egcfg_ptr->fsel_type;//当前文件模式

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_fileinfo(void* msg_ptr)
 * \获取当前播放的文件格式、总时间、比特率等信息
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   msg_ptr->msg.content.addr //存放文件信息
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_fileinfo(void* msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy当前文件信息到消息指针指向的数据区
    libc_memcpy((data_ptr->reply->content), &mengine_info.eg_file_info, sizeof(mmm_mp_file_info_t));

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_status(void* msg_ptr)
 * \获取当前播放状态
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   msg_ptr->msg.content.addr //存放状态信息
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_status(void* msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy当前播放状态到消息指针指向的数据区
    libc_memcpy(data_ptr->reply->content, &mengine_info.eg_status, sizeof(mengine_status_t));

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_playmode(void* msg_ptr)
 * \获取循环模式
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   msg_ptr->msg.content.addr //传送循环方式的指针
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_playmode(void* msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    //保存shuffle模式+循环模式
    *(uint8*) (data_ptr->reply->content) = (uint8) (mengine_info.eg_config.shuffle_flag
            | mengine_info.eg_config.repeat_mode);

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_get_playinfo(void* msg_ptr)
 * \获取当前播放时间和比特率
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   msg_ptr->msg.content.addr //存放播放时间和比特率
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_get_playinfo(void* msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy当前播放时间和比特率到消息指针指向的数据区
    libc_memcpy(data_ptr->reply->content, &mengine_info.eg_playinfo, sizeof(mengine_playinfo_t));

    //歌曲切换的时候进行保存
#ifdef CUR_FILE_SWITCH
    if ((mengine_info.eg_playinfo.cur_file_switch & 0x01) == 0x01)
    {
        sys_vm_write(&mengine_info.eg_config, VM_AP_MENGINE, sizeof(mengine_config_t));
    }
#endif

    //清除文件切换标志
    mengine_info.eg_playinfo.cur_file_switch &= (uint8) (0xfe);

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_fast_forward(void* msg_ptr)
 * \快进
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_fast_forward(void* msg_ptr)
{
    //当前播放状态
    mengine_status_t *eg_status = &mengine_info.eg_status;
    //当前不是AB复读
    //if (eg_status->ab_status == PlayAB_Null)
    //{
    //是正常播放 or 快进
    if ((eg_status->play_status == PlaySta) || (eg_status->play_status == PlayFast))
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 4);//快进
        eg_status->play_status = PlayFast;//修改状态
        eg_status->fast_status = FFPlaySta;
    }
    //}
    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_fast_backward(void* msg_ptr)
 * \快退
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_fast_backward(void* msg_ptr)
{
    mengine_status_t *eg_status = &mengine_info.eg_status;
    //当前不是AB复读
    //if (eg_status->ab_status == PlayAB_Null)
    //{
    //当前是正常播放 or 快退
    if ((eg_status->play_status == PlaySta) || (eg_status->play_status == PlayFast))
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) -4);//快退
        eg_status->play_status = PlayFast;//修改状态
        eg_status->fast_status = FBPlaySta;
    }
    //}

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_cancel_ffb(void* msg_ptr)
 * \取消快进退
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_cancel_ffb(void* msg_ptr)
{
    mengine_status_t *eg_status = &mengine_info.eg_status;
    //当前是快进退状态
    if ((eg_status->play_status == PlayFast) && (eg_status->fast_status != FBPlay_Null))
    {
        mmm_mp_cmd(mp_handle, MMM_MP_SET_FFB, (unsigned int) 0);//取消快进退
        eg_status->play_status = PlaySta;//修改状态
        eg_status->fast_status = FBPlay_Null;
    }

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e mengine_set_filepath(void* msg_ptr)
 * \设置播放文件路径信息
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_filepath_deal(void* msg_ptr)
{
    uint8 location_size;
    //void *data_ptr = msg_ptr->msg.content.addr;//消息数据指针
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    file_path_info_t *locat_info = (file_path_info_t*) data_ptr->msg.content.addr;//路径信息指针

    fsel_type_e file_source = locat_info->file_source;

    if ((file_source == FSEL_TYPE_PLAYLIST) || (file_source == FSEL_TYPE_LISTAUDIBLE) || (file_source
            == FSEL_TYPE_M3ULIST))
    {
        //是播放列表模式
        location_size = sizeof(plist_location_t);
    }
    else if ((file_source == FSEL_TYPE_COMMONDIR) || (file_source == FSEL_TYPE_DISKSEQUNCE))
    {
        //是目录模式
        location_size = sizeof(file_location_t);
    }
    else
    {
        //是收藏夹模式
        location_size = sizeof(flist_location_t);
    }
    libc_memcpy(&(mengine_info.eg_config.location), &(locat_info->file_path), (int) location_size);

    //mengine_info.eg_config.fsel_type = cur_type;
    /*_set_file();//设置播放文件*/
    return RESULT_IGNORE;

}

/******************************************************************************/
/*!
 * \par  Description:
 * \uinapp_result_e mengine_set_filepath(void* msg_ptr)
 * \设置播放文件路径信息
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
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

    //盘符
    uint8 disk_new, disk_old;
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    file_path_info_t *locat_info = (file_path_info_t*) data_ptr->msg.content.addr;//路径信息指针

    fsel_type_e cur_type = locat_info->file_source;
    plist_type_e cur_plist_type = mengine_info.eg_config.location.plist_location.list_type;

    if (locat_info->file_source == FSEL_TYPE_SDFILE)
    {
        ret_vals = _stop(STOP_NORMAL);//停止播放
        mengine_info.eg_config.fsel_type = locat_info->file_source;//更改引擎保存的文件选择类型
        libc_memcpy(g_file_name, locat_info->file_path.plist_location.filename, 12);
    }
    else
    {
        //注意_stop中有引用g_is_audible,因此要先停止歌曲播放
        ret_vals = _stop(STOP_NORMAL);//停止播放
        if (ret_vals == FALSE)
        {
            goto msg_end;
        }

        //文件选择类型修改，需要重新初始化文件选择器(盘符修改也需要重新初始化)
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
            //退出文件选择器
            fsel_exit();
            mengine_info.eg_config.fsel_type = cur_type;//更改引擎保存的文件选择类型
            ret_vals = mengine_file_init();//重新初始化
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
                ret_vals = mengine_file_init();//重新初始化
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
 * \改变播放文件路径
 * \param[in]    msg_ptr消息指针  para1
 * \param[in]    private_msg_t
 *               typedef struct
 *               {
 *                 unsigned char sync;		//同步标志，1B
 *                 sem_id_t sem_id; 		  //信号量ID，1B
 *                 msg_apps_t msg;		    //消息内容，6B
 *               } private_msg_t;
 *               typedef struct
 *               {
 *                 msg_apps_type_e  type;	//消息类型，2B
 *                 union
 *                 {
 *	                 unsigned char data[4];
 *                   unsigned char *addr;
 *                 }content;					//消息内容或其地址，由消息自己约定，4B
 *               } msg_apps_t;//私有消息队列的消息结构体

 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNORE 忽略
 * \retval       ......
 * \ingroup      mengine_event.c
 * \note
 */
/*******************************************************************************/
app_result_e mengine_change_filepath(void* msg_ptr)
{

    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    file_path_info_t *locat_info = (file_path_info_t*) data_ptr->msg.content.addr;//路径信息指针

    if (g_change_path_flag == 0)
    {
        g_path_type = mengine_info.eg_config.fsel_type;
    }
    mengine_filepath_deal(msg_ptr);

    mengine_info.eg_config.fsel_type = locat_info->file_source;
    //重新开始播放, 设置文件
    /*if (_set_file() == TRUE)
     {
     _play(PLAY_RESUME);//播放
     }*/
    g_change_path_flag = 1;
    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);
    return RESULT_IGNORE;
}

