/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-19 21:47     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_delete.c
 * \brief    gui ���������ļ���Ŀ¼ɾ���ؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ�����ļ���Ŀ¼ɾ�����ܡ�
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��applib��enhanced_bs��gui dialog���ȵȡ�
 * \version  1.0
 * \date     2011-9-19
 *******************************************************************************/

#include "common_ui.h"
#include "music_common.h"

bool delete_just_file(uint16 list_no, file_path_info_t *path_info, brow_del_e brow_del, bool del_playing) __FAR__;
app_result_e deleted_all(file_path_info_t *path_info, del_com_data_t *del_com) __FAR__;
app_result_e deleted_file_display(uint8 *filename);
void delete_string_format(string_desc_t *string, uint16 str_id, uint8 *file_name);

/******************************************************************************/
/*!
 * \par  Description:
 * \static _resume_music_after_delete( file_path_info_t *path_info)
 * \����music���Ų����״̬
 * \param[in]    path_info  �ļ�·��
 * \param[out]   none
 * \return       void the result
 * \ingroup
 * \note
 * \li   �����ǰ�޿ɲ�music�ļ�����ر�music����
 */
/*******************************************************************************/
static void _resume_music_after_delete(file_path_info_t *path_info)
{
    msg_apps_t msg;
    msg_reply_t reply;

    //�� manager ap ����Ϣ�ָ���̨���ֲ���
    msg.type = MSG_MENGINE_RESUME_SYNC;
    send_sync_msg(APP_ID_MENGINE, &msg, &reply, 0);

    if (reply.type == MSG_REPLY_FAILED)
    {
        //��ȡ��ǰ�����ļ���·����Ϣ
        reply.content = path_info;
        //��Ϣ����(����Ϣ����)
        msg.type = MSG_MENGINE_GET_FILEPATH_SYNC;

        send_sync_msg(APP_ID_MENGINE, &msg, &reply, 0);

        if (path_info->file_path.dirlocation.file_total == 0)
        {
            //ɱ��music��̨
            msg.type = MSG_KILL_APP_SYNC;
            msg.content.data[0] = APP_ID_MENGINE;
            send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
        }
    }

}
/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    ɾ���ļ���Ŀ¼�ؼ�������ʵ�֡�
 * \param[in]    path_info Ҫɾ���ļ���Ŀ¼����·���ṹ��
 * \param[in]    del_com ɾ�����ƽṹ��ָ��
 * \param[out]   path_info ɾ���ļ���Ŀ¼������µ�·���ṹ��
 * \return       app_result_e
 * \retval           RESULT_REDRAW ѯ���Ƿ�ɾ���Ի���ȡ������AP���ػ�UI��
 * \retval           RESULT_DELETE_FILE ɾ���ļ��ɹ����ء�
 * \retval           RESULT_DELETE_DIRLIST ɾ���ļ��л��б�ɹ����ء�
 * \retval           RESULT_DELETE_DIRLIST_NOSELF ɾ���ļ��л��б���ɾ�������ɹ����ء�
 * \retval           RESULT_ERROR ɾ���ļ�ʧ�ܷ��� ��
 * \retval           dialog other ֱ�ӷ��������� dialog ���ص���Ϣ����ϸ�μ� app_result_e ���塣
 * \retval           delete other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ���塣
 * \ingroup      controls
 * \par          exmaple code
 * \code
 del_com_data_t del_com;
 app_result_e result;

 fsel_browser_select(list_no);
 fsel_browser_get_location(&path_info.file_path, FSEL_TYPE_COMMONDIR);//path_info��֮ǰ�Ѿ���ʼ����
 del_com.filename = g_file_name;
 del_com.del_mode = DEL_MODE_FILE;
 del_com.del_playing = FALSE;
 del_com.del_self = TRUE;
 del_com.del_func = ebook_del_func;
 result = gui_delete(&path_info, &del_com);
 if(result == RESULT_DELETE_FILE)//ɾ���ļ��ɹ�����
 {
 //���ظ����б��
 }
 else if(result == RESULT_REDRAW)//ȡ��ɾ���������ػ�
 {

 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 * \li  �û�����ȷ���ļ�ϵͳ���ļ�ѡ�����Ѿ���ʼ���������� path_info ָ��Ҫɾ�����ļ����б�
 *******************************************************************************/
app_result_e gui_delete(file_path_info_t *path_info, del_com_data_t *del_com)
{
    msg_apps_t msg;
    dialog_com_data_t dialog_com;
    string_desc_t delete_ask_str;
    app_result_e result = RESULT_NULL;
    bool ret, restart_music = FALSE;
    uint8 tag_backup;
    gui_direct_e tmp_direct;

    tag_backup = g_this_app_timer_tag;//get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_DELETE);
    tmp_direct = com_get_gui_direction();

    //���ļ���ת��Ϊ "����..." ��ʽ�������� gui_delete ����������Ч
    com_ellipsis_to_longname(del_com->filename, com_name_buffer, 52);

    //ѯ���Ƿ�Ҫɾ���ļ���Ŀ¼
    delete_ask_str.data.str = com_str_buffer;

    //��ʼ���Ի���
    dialog_com.dialog_type = DIALOG_BUTTON_INCLUDE;
    dialog_com.button_type = BUTTON_YESNO;
    dialog_com.icon_id = -1;
    dialog_com.string_desc = &delete_ask_str;
    dialog_com.active_default = 1;

    //for 16KB page nandflash
    if ((g_app_info_state.type == ENGINE_MUSIC)//���ڲ�������
            && (g_app_info_state.state == ENGINE_STATE_PLAYING) && ((g_this_app_info->app_id != APP_ID_MUSIC)
            || (del_com->del_playing != TRUE)))//����ɾ�����ڲ��ŵ������ļ�
    {
        delete_string_format(&delete_ask_str, S_CLOSE_MUSIC_DEL, com_name_buffer);

        //ѯ���Ƿ� �ȹر����ֲ���������ɾ���ļ�
        result = gui_dialog(DIALOG_ASK, &dialog_com);
        if (result != RESULT_DIALOG_YES)
        {
            g_this_app_timer_tag = tag_backup;//change_app_timer_tag(tag_backup);
            return result;
        }

        //�� manager ap ����ϢҪ���̨�˳�
        msg.type = MSG_MENGINE_PAUSE_SYNC;
        send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0);

        restart_music = TRUE;//ɾ����ɺ�ָ����ֲ���
    }
    else
    {
        delete_string_format(&delete_ask_str, S_DEL_FILE_ASK, com_name_buffer);

        //ѯ���Ƿ� ɾ���ļ�
        if (tmp_direct == GUI_DIRECT_NORMAL)//��������
        {
            result = gui_dialog(DIALOG_ASK, &dialog_com);
            if (result != RESULT_DIALOG_YES)
            {
                g_this_app_timer_tag = tag_backup;//change_app_timer_tag(tag_backup);
                return result;
            }
        }
        /*else//����ģʽ
         {
         result = gui_dialog (DIALOG_ASK_V, &dialog_com);
         if(result != RESULT_DIALOG_YES)
         {
         g_this_app_timer_tag = tag_backup;//change_app_timer_tag(tag_backup);
         return result;
         }
         }*/
    }

    //��ʾ ����ɾ���ļ�
    if (tmp_direct == GUI_DIRECT_NORMAL)//��������
    {
        gui_dialog_msg(DIALOG_MSG, DIALOG_INFOR_IMMEDIATE, S_DEL_FILE_ING);
    }
    else//����ģʽ
    {
        // gui_dialog_msg (DIALOG_MSG_V, DIALOG_INFOR_IMMEDIATE, S_DEL_FILE_ING);
    }

    //ִ��ɾ���ص�����
    if (del_com->del_func != NULL)
    {
        del_com->del_func();
    }

    //ȷ��ɾ����ɾ���ļ�
    if (del_com->del_mode == DEL_MODE_FILE)
    {
        //ִ��ɾ���ļ�
        ret = delete_just_file(0, path_info, BROW_DEL_FILE, del_com->del_playing);
        if (ret == TRUE)
        {
            result = deleted_file_display(com_name_buffer);
            if ((result == RESULT_NULL) || (result == RESULT_REDRAW))
            {
                result = RESULT_DELETE_FILE; //ɾ���ļ���ɷ���
            }
        }
        else
        {
            result = RESULT_ERROR;
        }
    }
    //ȷ��ɾ����ɾ���ļ��л򲥷��б�
    else
    {
        result = deleted_all(path_info, del_com);
        if (result == RESULT_DELETE_DIRLIST)
        {
            result = deleted_file_display(com_name_buffer);
            if (result == RESULT_NULL)
            {
                if (del_com->del_self == TRUE)
                {
                    result = RESULT_DELETE_DIRLIST; //ɾ���ļ��л��б���ɷ���
                }
                else
                {
                    result = RESULT_DELETE_DIRLIST_NOSELF; //ɾ���ļ��л��б���ɾ��������ɷ���
                }
            }
        }
    }

    //�ָ����ֲ���
    if (restart_music == TRUE)
    {
        //�� manager ap ����Ϣ�ָ���̨���ֲ���
        _resume_music_after_delete(path_info);
    }
    g_this_app_timer_tag = tag_backup;//change_app_timer_tag(tag_backup);
    return result;
}

/*! \endcond */

/*! \cond */

/******************************************************************************/
/*!
 * \par  Description:
 *    ��ʾ��ɾ����������ʾ��
 * \param[in]    path_info Ҫɾ���ļ���Ŀ¼����·���ṹ��
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_NULL ������ʾ���������󷵻�AP
 * \retval           RESULT_REDRAW Ӧ����Ϣ������ RESULT_REDRAW��������other2 ��
 * \retval           other1 ������ʾ��ֹ���أ����û�gui�������ֹ������key_map_listӳ�䣩
 * \retval           other2 �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \note
 *******************************************************************************/
app_result_e deleted_file_display(uint8 *filename)
{
    style_infor_t style_infor1, style_infor2;
    textbox_private_t deleted_infor;
    string_desc_t delete_str;
    animation_com_data_t deleted_anim;
    app_result_e result;
    uint8 mode_backup, icon_backup;
    gui_direct_e tmp_direct;

    gui_get_headbar_mode(&mode_backup, &icon_backup);
    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);
    tmp_direct = com_get_gui_direction();

    //style_infor.type = UI_COM;
    style_infor1.type = UI_COM;
    style_infor2.type = UI_COM;

    //��ʾ����ͼƬ
    if (tmp_direct == GUI_DIRECT_NORMAL)//��������
    {
        //style_infor.style_id = DELETED_BG;
        style_infor1.style_id = DELETED_INFOR;
        style_infor2.style_id = DELETED_ANIM;
    }

    //ui_show_picbox(&style_infor, NULL);
    ui_set_backgd_color(Color_BACKCOLOR_def);
    ui_clear_screen(NULL); //2015-10-30

    //��ʾ����ɾ���ɹ���
    delete_str.data.str = com_str_buffer;
    delete_string_format(&delete_str, S_DEL_FILE_OK, filename);

    deleted_infor.lang_id = UNICODEDATA;
    deleted_infor.str_value = com_str_buffer;
    deleted_infor.str_id = -1;
    ui_show_textbox(&style_infor1, &deleted_infor, TEXTBOX_DRAW_NORMAL);

    //��ʾɾ���ɹ�����
    deleted_anim.direction = DIRECTION_NORMAL;
    deleted_anim.interval = 250;
    deleted_anim.forbid = 0;
    deleted_anim.callback = NULL;
    result = gui_animation(&style_infor2, &deleted_anim);

    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ����ɾ��ѯ���ַ�����
 * \param[in]    str_id ɾ��ѯ�ʶ�������ַ���ID
 * \param[in]    file_name Ҫɾ�����ļ�����
 * \param[out]   string ɾ��ѯ���ַ���������
 * \return       none
 * \note
 *******************************************************************************/
void delete_string_format(string_desc_t *string, uint16 str_id, uint8 *file_name)
{
    string_desc_t temp_string;

    string->argv = str_id;
    string->language = UNICODEDATA;
    string->length = 128;

    temp_string.data.str = file_name;
    temp_string.length = -1;
    if ((file_name[0] == 0xff) && (file_name[1] == 0xfe))
    {
        temp_string.language = UNICODEDATA;
    }
    else
    {
        temp_string.language = ANSIDATAAUTO;
    }

    com_string_format(string, &temp_string);
}

/*! \endcond */
