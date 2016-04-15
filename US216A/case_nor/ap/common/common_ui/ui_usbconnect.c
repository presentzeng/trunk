/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-menulist for usb menu
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-16 9:07     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_usbconnect.c
 * \brief    commonUI ר����usbѡ��Ի���ؼ�������ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ��ר����usbѡ��Ի���GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *
 * \version 1.0
 * \date  2011-9-16
 *******************************************************************************/

#include "common_ui.h"

#define USB_OPTION_COUNT    2
const uint16 usb_buttons[USB_OPTION_COUNT] =
{ S_LINK_MODE_TRANS, S_LINK_MODE_PLAY };
const uint16 usb_result[USB_OPTION_COUNT] =
{ RESULT_USB_TRANS, RESULT_CHARGE_PLAY };

const uint16 style_usb_time[5] =
{ USB_TIME_ONE, USB_TIME_TWO, USB_TIME_THREE, USB_TIME_FOUR, USB_TIME_FIVE };

void timer_usb_handle(void)
{
    style_infor_t style_infor;

    style_infor.style_id = style_usb_time[g_usb_auto_select];
    style_infor.type = UI_COM;
    //��ʾ��ʱ����
    ui_show_picbox(&style_infor, NULL);
    g_usb_auto_select++;
}

void clear_usb_time(void)
{
    style_infor_t style_infor;

    style_infor.style_id = USB_TIME_CLEAR;
    style_infor.type = UI_COM;
    //��ʾ��ʱ����
    ui_show_picbox(&style_infor, NULL);
}

//usbconnect gui app msg deal
app_result_e usbconnect_msg_deal(dialog_com_data_t *dialog_com, dialog_private_t *dialog_private)
{
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    app_result_e result = RESULT_NULL;

    //��ȡ�� gui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)
    {
        //���� gui��Ϣӳ��
        if (com_key_mapping(&input_msg, &gui_event, usbconnect_key_map_list) == TRUE)
        {
            switch (gui_event)
            {
                case EVENT_DIALOG_NEXT_BUTTON:
                //ѡ����һ����ťѡ��
                dialog_private->old = dialog_private->active;
                if (dialog_private->active < (dialog_private->button_cnt - 1))
                {
                    dialog_private->active++;
                }
                else
                {
                    dialog_private->active = 0;
                }
                dialog_com->draw_mode = DIALOG_DRAW_BUTTON;

                //�޸���ʱ��ʱ��
                clear_usb_time();
                g_usb_auto_select = 0;
                restart_app_timer(g_usb_timer_id);
                break;

                case EVENT_DIALOG_PREV_BUTTON:
                //ѡ����һ����ťѡ��
                dialog_private->old = dialog_private->active;
                if (dialog_private->active > 0)
                {
                    dialog_private->active--;
                }
                else
                {
                    dialog_private->active = dialog_private->button_cnt - 1;
                }
                dialog_com->draw_mode = DIALOG_DRAW_BUTTON;

                //�޸���ʱ��ʱ��
                clear_usb_time();
                g_usb_auto_select = 0;
                restart_app_timer(g_usb_timer_id);
                break;

                case EVENT_DIALOG_CONFIRM:
                //ȷ��ѡ���
                result = usb_result[dialog_private->active];
                break;

                case EVENT_DIALOG_CANCEL:
                //ȡ�����أ���Ĭ�Ϸ�ʽ����--���Ͳ���
                result = RESULT_CHARGE_PLAY;
                break;

                default:
                result = com_message_box(gui_event);
                break;
            }
        }
    }
    //���ˣ�gui��Ϣ�����Ѿ�������ϣ���ȡap˽����Ϣ��ϵͳ��Ϣ������ת��Ϊ˽����Ϣ�ٷ��أ�������
    else
    {
        if (get_app_msg(&private_msg) == TRUE)
        {
            result = g_this_app_msg_dispatch(&private_msg);
            if (result == RESULT_REDRAW)
            {
                dialog_com->draw_mode = DIALOG_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
            }
            else if (result != RESULT_NULL)
            {
                if (result == RESULT_USB_UNSTICK)
                {
                    result = RESULT_REDRAW;
                }
            }
            else
            {
                //do nothing for QAC
            }
        }
    }

    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �Ի���ؼ����������������ǶԻ���ؼ����ⲿ�ӿ�
 * \param[in]    style_id ui editor������ƵĶԻ���ؼ�ģ��
 * \param[in]    dialog_com ָ��Ի���ؼ���ʼ���ṹ����������ϲ�ԶԻ���Ŀ�����������
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_USB_TRANS ѡ��USB���ݴ���ģʽ
 * \retval           RESULT_USB_PLAY ѡ��USB��粥��ģʽ����ȡ������Ĭ�����ӷ�ʽ
 * \retval           RESULT_REDRAW ��Ч���أ�AP��Ҫ�ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \note
 *******************************************************************************/
app_result_e ui_dialog_usbconnect(uint16 style_id, dialog_com_data_t *dialog_com)
{
    dialog_private_t dialog_private;
    style_infor_t style_infor;
    app_result_e result;
    uint8 i;

    //bank data ��ʼ��
    g_usb_auto_select = 0;

    usb_selecting_state = TRUE;
    dialog_private.icon_id = dialog_com->icon_id;
    dialog_private.string_desc = dialog_com->string_desc;

    style_infor.style_id = style_id;
    style_infor.type = UI_COM;

    dialog_com->draw_mode = DIALOG_DRAW_ALL;

    //�滻��ť�ַ���
    dialog_private.button_cnt = USB_OPTION_COUNT;
    for (i = 0; i < dialog_private.button_cnt; i++)
    {
        dialog_private.buttons[i] = usb_buttons[i];
    }
    dialog_private.old = dialog_private.active = dialog_com->active_default;

    //����Ի�����Ϣѭ��
    while (1)
    {
        //ˢ��dialogbox
        if (dialog_com->draw_mode != DIALOG_DRAW_NULL)
        {
            ui_show_dialogbox(&style_infor, &dialog_private, (uint32) dialog_com->draw_mode);
            //��ʼʱ���� USBʱ�� ����ͼƬ
            if (dialog_com->draw_mode == DIALOG_DRAW_ALL)
            {
                clear_usb_time();
            }

            dialog_com->draw_mode = DIALOG_DRAW_NULL;
        }

        //�Ѿ�5�룬ѡ��ǰ������
        if (g_usb_auto_select == 5)
        {
            //ȷ��ѡ���
            result = usb_result[dialog_private.active];
            break;
        }

        result = usbconnect_msg_deal(dialog_com, &dialog_private);
        if (result != RESULT_NULL)
        {
            break;
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    usb_selecting_state = FALSE;
    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    usbѡ��Ի���
 * \param[in]    none
 * \param[out]   none
 * \return       app_result_e��ͬ ui_dialog_usbconnect һ�£�
 * \retval           RESULT_APP_QUIT ѡ��USB���ݴ���ģʽ���˳���ǰǰ̨AP
 * \retval           RESULT_USB_TRANS ��config ap�У�ѡ��USB���ݴ���ģʽ��ֻ������ config ap
 * \retval           RESULT_REDRAW ѡ��USB��粥��ģʽ �� ��Ч���أ�AP��Ҫ�ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \note
 *******************************************************************************/
app_result_e gui_usbconnect(void)
{
    dialog_com_data_t dialog_com;
    string_desc_t string_desc;
    private_msg_t private_msg;
    app_result_e result;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;
    gui_direct_e direct_bk;

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_USBCN);

    direct_bk = com_get_gui_direction();
    com_set_gui_direction(GUI_DIRECT_NORMAL);

    gui_get_headbar_mode(&mode_backup, &icon_backup);
    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //��ʼ�����Ʊ�������������ʱ��
    g_usb_auto_select = 0;
    g_usb_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 1000, timer_usb_handle);

    //��ʾusbѡ��Ի���ѡ������ģʽ
    string_desc.data.id = S_CHOOSE_LINK_MODE;
    string_desc.language = UNICODEID;
    dialog_com.dialog_type = DIALOG_BUTTON_INCLUDE;
    dialog_com.button_type = BUTTON_YESNO;
    dialog_com.icon_id = V_U16_INVALID;
    dialog_com.string_desc = &string_desc;
    dialog_com.active_default = 0;//Ĭ��ѡ�� ������봫�䡱

    //draw bg
    ui_set_backgd_color(Color_BACKCOLOR_def);//��Ϊȫ���� //2015-11-09hjh
    ui_clear_screen(NULL); //2015-11-09

    result = ui_dialog_usbconnect(DIALOG_USB, &dialog_com);
    if (result == RESULT_USB_TRANS)//ѡ��USB���ݴ���ģʽ
    {
        if (g_this_app_info->app_id != APP_ID_CONFIG)//���ڿ���Ӧ����
        {
            //usbѡ�����ݴ���ģʽ��֪ͨ manager ap ����Udisk ���ݴ���
            msg_apps_t msg;

            //�� manager AP ���� MSG_USB_TRANS �첽��Ϣ���� manager ap���ƽ��� Udisk ap
            msg.type = MSG_USB_TRANS;
            send_async_msg(APP_ID_MANAGER, &msg);

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
        }
    }
    else if (result == RESULT_CHARGE_PLAY)//ѡ��USB��粥��ģʽ
    {
        //usbѡ���粥��ģʽ��������磬����ԭ�Ƚ���
        key_chargeset(CHARGE_START, 0, 0);
        result = RESULT_REDRAW;
    }
    else
    {
        //do nothing for QAC
    }

    //������ʱ��
    kill_app_timer(g_usb_timer_id);

    com_set_gui_direction(direct_bk);
    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);

    return result;
}
