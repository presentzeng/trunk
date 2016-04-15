/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-keylock
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-19 16:52     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_keylock.c
 * \brief    commonUI ����������ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�ְ�����GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�picbox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2011-9-19
 *******************************************************************************/

#include "common_ui.h"

//gui app msg deal
app_result_e keylock_msg_deal(key_lock_mode_e *this_lock_mode, key_lock_mode_e *last_lock_mode)
{
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    app_result_e result = RESULT_NULL;

    //��ȡgui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)//��gui��Ϣ
    {
        //���а���ӳ��
        if (com_key_mapping(&input_msg, &gui_event, keylock_list) == TRUE)
        {
            switch (gui_event)
            {
                case EVENT_ENTER_KEYLOCK:
                switch (*last_lock_mode)
                {
                    case KEY_LOCK_MODE_HOLD:
                    if (get_keylock_state() == TRUE)
                    {
                        *this_lock_mode = KEY_LOCK_MODE_UNLOCKING;
                    }
                    else
                    {
                        *this_lock_mode = KEY_LOCK_MODE_LOCKING;
                    }
                    break;

                    case KEY_LOCK_MODE_LOCKING:
                    *this_lock_mode = KEY_LOCK_MODE_UNLOCKING;
                    break;

                    default:
                    *this_lock_mode = KEY_LOCK_MODE_LOCKING;
                    break;
                }

                *last_lock_mode = *this_lock_mode;
                result = RESULT_REDRAW;//ǿ���˳���2��whileѭ��
                break;

                default:
                result = com_message_box(gui_event);
                break;
            }
        }
    }
    else
    {
        if (get_app_msg(&private_msg) == TRUE)
        {
            result = g_this_app_msg_dispatch(&private_msg);
        }
    }

    return result;
}

//keylock_handle
void keylock_handle(void)
{
    g_kl_time_is_up = TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ���������������ǰ�����ģ����ⲿ�ӿ�
 * \param[in]    lockkey �Ƿ��¡����������룬TRUE��ʾ�ǣ�FALSE��ʾ��
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_REDRAW ��������AP���ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \note
 *******************************************************************************/
app_result_e gui_keylock(bool lockkey)
{
    app_result_e result = RESULT_NULL;
    animation_com_data_t anm_com;
    style_infor_t style_infor;
    key_lock_mode_e this_lock_mode, last_lock_mode;
    int8 wait_timer;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;
    gui_direct_e direct_bk;

    //��ʼ���������� UI ״̬
    if (lockkey == TRUE)
    {
        if (get_keylock_state() == TRUE)
        {
            this_lock_mode = KEY_LOCK_MODE_UNLOCKING;
        }
        else
        {
            this_lock_mode = KEY_LOCK_MODE_LOCKING;
        }
    }
    else
    {
        this_lock_mode = KEY_LOCK_MODE_HOLD;
    }
    last_lock_mode = this_lock_mode;

    //config, playlist, udisk, ��Ӧ�ý��޸İ�����״̬�������˳�
    if ((g_this_app_info->app_id == APP_ID_CONFIG) || (g_this_app_info->app_id == APP_ID_PLAYLIST)
            || (g_this_app_info->app_id == APP_ID_UDISK))
    {
        //��������״̬
        if (last_lock_mode == KEY_LOCK_MODE_LOCKING)
        {
            change_keylock_state(TRUE);
        }
        else if (last_lock_mode == KEY_LOCK_MODE_UNLOCKING)
        {
            change_keylock_state(FALSE);
        }
        else
        {
            //do nothing for QAC
        }

        return RESULT_NULL;
    }

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_KEYLK);

    direct_bk = com_get_gui_direction();
    com_set_gui_direction(GUI_DIRECT_NORMAL);

    gui_get_headbar_mode(&mode_backup, &icon_backup);
    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //��ʱ����������Ӧ����
    change_keylock_state(FALSE);

    //ˢ����ͼ
    /*style_infor.style_id = LOCK_BG;
     style_infor.type = UI_COM;
     ui_show_picbox(&style_infor, NULL);*/
    ui_set_backgd_color(Color_BACKCOLOR_def);
    ui_clear_screen(NULL); //2015-10-30

    while (1)
    {
        //���ư�����UI
        if ((this_lock_mode == KEY_LOCK_MODE_LOCKING) || (this_lock_mode == KEY_LOCK_MODE_UNLOCKING))
        {
            anm_com.interval = 250;

            if (this_lock_mode == KEY_LOCK_MODE_LOCKING)
            {
                anm_com.direction = DIRECTION_NORMAL;
            }
            else
            {
                anm_com.direction = DIRECTION_REVERSE;
            }

            anm_com.forbid = TRUE;//������ֹ
            anm_com.key_map_list = keylock_list;
            anm_com.callback = NULL;

            style_infor.style_id = LOCK_ANIMATION;
            style_infor.type = UI_COM;
            result = gui_animation(&style_infor, &anm_com);
            if ((result != RESULT_NULL) && (result != RESULT_REDRAW))
            {
                //������ʾ��ֹ���أ����û�gui�������ֹ��
                if (result == EVENT_ENTER_KEYLOCK)
                {
                    if (this_lock_mode == KEY_LOCK_MODE_UNLOCKING)
                    {
                        this_lock_mode = KEY_LOCK_MODE_LOCKING;
                    }
                    else
                    {
                        this_lock_mode = KEY_LOCK_MODE_UNLOCKING;
                    }
                    last_lock_mode = this_lock_mode;
                    continue;
                }
                else//����ԭ�򷵻�
                {
                    break;//�˳��ؼ�
                }
            }
        }
        else
        {
            style_infor.style_id = LOCK_HOLD;
            style_infor.type = UI_COM;
            ui_show_picbox(&style_infor, NULL);
        }

        //�ȴ�2���Ӻ��˳����ڴ��ڼ�����ְ��°�����������һ�δ�������������
        wait_timer = set_app_timer(APP_TIMER_ATTRB_UI, 2000, keylock_handle);
        this_lock_mode = KEY_LOCK_MODE_NULL;
        g_kl_time_is_up = FALSE;
        while (1)
        {
            if (g_kl_time_is_up == TRUE)
            {
                //��������
                result = RESULT_REDRAW;
                break;
            }

            result = keylock_msg_deal(&this_lock_mode, &last_lock_mode);
            if (result != RESULT_NULL)
            {
                break;
            }

            //����10ms�����������
            sys_os_time_dly(1);
        }
        kill_app_timer(wait_timer);
        if (this_lock_mode == KEY_LOCK_MODE_NULL)
        {
            break;
        }
    }

    //��������״̬
    if ((last_lock_mode == KEY_LOCK_MODE_LOCKING) || (last_lock_mode == KEY_LOCK_MODE_HOLD))
    {
        change_keylock_state(TRUE);
    }
    else//KEY_LOCK_MODE_UNLOCKING
    {
        change_keylock_state(FALSE);
    }

    com_set_gui_direction(direct_bk);
    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}
