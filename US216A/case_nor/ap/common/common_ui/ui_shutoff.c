/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-shutoff
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-19 10:55     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_shutoff.c
 * \brief    commonUI �ػ�ȷ���ؼ�ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�ֹػ�ȷ��GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�picbox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2011-9-19
 *******************************************************************************/

#include "common_ui.h"

void show_shutoff_time(uint8 time)
{
    style_infor_t style_infor;
    picbox_private_t picbox_data;

    picbox_data.pic_id = V_U16_INVALID;
    picbox_data.frame_id = time;

    style_infor.style_id = SHUTOFF_TIME;
    style_infor.type = UI_COM;

    //��ʾ��ʱ����
    ui_show_picbox(&style_infor, &picbox_data);
}

void timer_shutoff_handle(void)
{
    show_shutoff_time(g_so_time);
    g_so_time++;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �ػ�ȷ�����������ǹػ�ȷ�����ⲿ�ӿ�
 * \param[in]    none
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_NULL ����3 �룬�㲥 POWER_OFF ��Ϣ�󷵻�
 * \retval           RESULT_REDRAW PLAY �������𷵻أ�����AP �ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \note
 *******************************************************************************/
app_result_e gui_shut_off(void)
{
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    style_infor_t style_infor;
    int8 timer_id_shutoff;
    app_result_e result = RESULT_NULL;
    uint8 frame_cnt;
    bool ret;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;
    gui_direct_e direct_bk;

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_SHUT);

    direct_bk = com_get_gui_direction();
    com_set_gui_direction(GUI_DIRECT_NORMAL);

    gui_get_headbar_mode(&mode_backup, &icon_backup);
    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //ˢ����
    ui_set_backgd_color(Color_BACKCOLOR_def);
    ui_clear_screen(NULL);

    //��ȡ��ʱ��֡ͼƬ֡��
    style_infor.style_id = SHUTOFF_TIME;
    style_infor.type = UI_COM;
    ui_get_picbox_attrb(&style_infor, &frame_cnt, 1);

    //ˢ1sͼƬ
    g_so_time = 0;
    timer_shutoff_handle();

    //������ʱ��
    timer_id_shutoff = set_app_timer(APP_TIMER_ATTRB_UI, 1000, timer_shutoff_handle);

    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //����Ի�����Ϣѭ��
    while (1)
    {
        //�ػ�ȷ��ʱ���ѵ�
        if (g_so_time >= frame_cnt)
        {
            msg_apps_t msg;

            //�㲥�ػ���Ϣ
            msg.type = MSG_POWER_OFF;
            broadcast_msg(&msg);

            //���κ��� play ��������ʵ�ֽ��������󰴰����˳����
            com_filter_key_hold();

            //�ȴ�AP�˳���Ϣ MSG_APP_QUIT ���ٷ��� RESULT_APP_QUIT��Ӧ��ֱ���˳�
            while (1)
            {
                //����10ms�����������
                sys_os_time_dly(1);
                if ((get_app_msg(&private_msg) == TRUE) && (private_msg.msg.type == MSG_APP_QUIT))
                {
                    result = RESULT_APP_QUIT;
                    break;
                }
            }
            break;
        }

        //��ȡgui��Ϣ
        ret = get_gui_msg(&input_msg);
        if (ret == TRUE)//��gui��Ϣ
        {
            //���а���ӳ��
            if (com_key_mapping(&input_msg, &gui_event, shutoff_key_map_list) == TRUE)
            {
                //GUI������Ϣ����
                if (gui_event == EVENT_SHUTOFF_QUIT)
                {
                    //�ػ��������ػ�ԭ��UI
                    result = RESULT_REDRAW;
                    ui_set_backgd_color(Color_BACKCOLOR_def); //�ػ�δ�ɵĻ�����Ҫ�Ȱѵ�ǰUI���
				    ui_clear_screen(NULL);  //2015-10-30
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
                if (result != RESULT_NULL)
                {
                    break;
                }
            }
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    kill_app_timer(timer_id_shutoff);
    com_set_gui_direction(direct_bk);
    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}
