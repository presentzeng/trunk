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
 * \file     _ui_delete_sub.c
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

bool delete_just_file(uint16 list_no, file_path_info_t *path_info, brow_del_e brow_del, bool del_playing);

/*! \cond */

/******************************************************************************/
/*!
 * \par  Description:
 *    ɾ��Ŀ¼�򲥷��б�
 * \param[in]    path_info Ҫɾ��Ŀ¼��·���ṹ��
 * \param[in]    del_com ɾ�����ƽṹ��ָ��
 * \param[out]   path_info ɾ��Ŀ¼����µ�·���ṹ��
 * \return       app_result_e
 * \retval           RESULT_REDRAW ѯ���Ƿ�ɾ���Ի���ȡ������AP���ػ�UI��
 * \retval           RESULT_DELETE_FILE ɾ���ļ��ɹ����ء�
 * \retval           RESULT_DELETE_DIRLIST ɾ���ļ��л��б�ɹ����ء�
 * \retval           RESULT_DELETE_DIRLIST_NOSELF ɾ���ļ��л��б���ɾ�������ɹ����ء�
 * \retval           RESULT_ERROR ɾ���ļ�ʧ�ܷ��� ��
 * \retval           dialog other ֱ�ӷ��������� dialog ���ص���Ϣ����ϸ�μ� app_result_e ���塣
 * \retval           delete other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ���塣
 * \note
 *******************************************************************************/
app_result_e deleted_all(file_path_info_t *path_info, del_com_data_t *del_com)
{
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    app_result_e result = RESULT_NULL;
    bool info_flag = TRUE;
    bool ret;

    dir_brow_t browser_dir_browser;
    scanfile_result_e browser_result;

    //�Ƚ���Ҫɾ�����ļ��У������뵱ǰ���ļ���
    browser_dir_browser.name_buf = com_str_buffer;
    browser_dir_browser.name_len = 64;
    fsel_browser_enter_dir(CUR_DIR, 0, &browser_dir_browser);

    //�ļ��б��������ʼ��
    fsel_browser_scanfile_init(&browser_dir_browser);
    //�����ļ��в�ɾ���ļ������ļ���
    while (1)
    {
        browser_result = fsel_browser_scanfile(path_info, &browser_dir_browser, SCANFILE_TYPE_DELETE);
        if (browser_result == SCANFILE_RESULT_FILE)//�����ļ�
        {
            //ɾ����ǰ�ļ�
            if (delete_just_file(0, path_info, BROW_DEL_FILE, FALSE) == FALSE)
            {
                return RESULT_ERROR;
            }
        }
        else//�����ļ��У�SONDIR ���ļ��� PARENTDIR ���ļ��� SELF �ļ�������
        {
            //����ļ���Ϊ�գ�ɾ��֮
            if (browser_result == SCANFILE_RESULT_PARENTDIR)
            {
                //���ص���Ŀ¼�������Ŀ¼����ɾ��
                if (fsel_browser_delete(BROW_DEL_DIR, 1, &(path_info->file_path)) == FALSE)
                {
                    return RESULT_ERROR;
                }
                //���µ�ǰ���ļ�Ŀ¼����
                browser_dir_browser.dir_total--;
            }

            if (browser_result == SCANFILE_RESULT_SELF)//�ļ�������
            {
                //ɾ�������ļ���ϣ���ʾ��ɾ���������˳�Ӧ��
                if (del_com->del_self == TRUE)
                {
                    if (fsel_browser_delete(BROW_DEL_DIR, del_com->del_no, &(path_info->file_path)) == FALSE)
                    {
                        return RESULT_ERROR;
                    }
                }

                result = RESULT_DELETE_DIRLIST; //ɾ���ļ��л��б���ɷ���
                break;//�����˳�
            }
        }

        //ֻ�����һ��ɾ��ʱ��ʾ��Ϣ
        if (info_flag == TRUE)
        {
            info_flag = FALSE;
        }

        //��ȡgui��Ϣ
        ret = get_gui_msg(&input_msg);
        if (ret == TRUE)//��gui��Ϣ
        {
            //���а���ӳ��
            if (com_key_mapping(&input_msg, &gui_event, delete_key_map_list) == TRUE)
            {
                //GUI������Ϣ����
                switch (gui_event)
                {
                    case EVENT_DELETE_STOP:
                    //ȡ������
                    return RESULT_REDRAW;

                    default:
                    break;
                }
            }
        }
        else
        {
            //���ˣ�gui��Ϣ�����Ѿ��������
            //��ȡap˽����Ϣ��ϵͳ��Ϣ������ת��Ϊ˽����Ϣ�ٷ��أ���������
            ret = get_app_msg(&private_msg);
            if (ret == TRUE)
            {
                result = g_this_app_msg_dispatch(&private_msg);
                if (result == RESULT_REDRAW)
                {
                    info_flag = TRUE;
                }
                else if (result != RESULT_NULL)
                {
                    break;
                }
                else
                {
                    //do nothing for QAC
                }
            }
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ɾ����ǰ�ļ�����ļ���
 * \param[in]    list_no Ҫɾ�����ļ����
 * \param[in]    path_info ָ��ǰҪɾ�����ļ���Ŀ¼
 * \param[in]    brow_del ɾ���������ͣ�BROW_DEL_FILE��ʾɾ���ļ���BROW_DEL_DIR��ʾɾ����Ŀ¼
 * \param[in]    del_playing �Ƿ�ɾ�����ڲ����ļ�
 * \param[out]   none
 * \return       bool
 * \retval           �����Ƿ�ɾ���ļ��ɹ���true��ʾ�ɹ���false��ʾʧ��
 * \note
 *******************************************************************************/
bool delete_just_file(uint16 list_no, file_path_info_t *path_info, brow_del_e brow_del, bool del_playing)
{
    msg_apps_t msg;
    bool ret;

    //1.֪ͨ��������Ҫɾ���ļ������ɾ����ǰ���ڲ����ļ�����Ҫ��ֹͣ����
    if (g_app_info_state.type == ENGINE_MUSIC)
    {
        if (del_playing == TRUE)
        {
            msg.type = MSG_MENGINE_DELFILE_MUSUI_SYNC;
        }
        else
        {
            msg.type = MSG_MENGINE_DELFILE_OTER_SYNC;
        }
        msg.content.addr = path_info;
        if (send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0) == FALSE)
        {
            return FALSE;
        }
    }

    //2.ɾ����ǰ�ļ�����ļ���
    ret = fsel_browser_delete(brow_del, list_no, &(path_info->file_path));

    return ret;
}

/*! \endcond */
