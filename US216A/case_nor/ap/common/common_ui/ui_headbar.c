/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-19 21:28     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_headbar.c
 * \brief    gui ��������״̬���ؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�֣�������״̬���ؼ���
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��ui������applib���ȵȡ�
 * \version  1.0
 * \date     2011-9-19
 *******************************************************************************/

#include "common_ui.h"

#define _HEADBAR_PRINTD_INFO_INT(a,b)    // PRINTD_INFO_INT(a,b)
#define _HEADBAR_PRINTD_DBG(a)           // PRINTD_INFO(a)
static void __show_battery(uint8 display);

/******************************************************************************/
/*!
 * \par  Description:
 *    ����״̬��ģʽ�ȡ�
 * \param[in]    mode ״̬��ģʽ���� headbar_mode_e  ö�����Ͷ��塣
 * \param[in]    icon_id Ӧ��ͼ��ID���ο� headbar_icon_e ö�����Ͷ��塣
 * \param[out]   none
 * \return       none
 * \ingroup      controls
 * \note
 * \li  һ������£�Ӧ��ֻ���ڳ�ʼ��ʱ�е��øýӿ�����ģʽ��
 *******************************************************************************/
void gui_set_headbar_mode(headbar_mode_e mode, headbar_icon_e icon_id)
{
    this_headbar_mode = mode;
    this_headbar_icon_id = icon_id;

    switch (mode)
    {
        case HEADBAR_MODE_NORMAL://Ĭ��ģʽ����ʾӦ��ͼ��͵��ͼ��
        this_headbar_style.icon_sty = HEADBAR_ICON; //2015-11-03
        //this_headbar_style.icon_sty = V_U16_INVALID;
        this_headbar_style.battery_sty = HEADBAR_BATTERY;
        this_headbar_style.bg_sty = V_U16_INVALID;
        this_headbar_style.time_sty = V_U16_INVALID;
        this_headbar_style.cable_sty = V_U16_INVALID;
        this_headbar_style.card_sty = HEADBAR_CARD;

        g_headbar_update = HEADBAR_UPDATE_INIT;
        restart_app_timer(sys_status_timer_id);
        break;

        default:
        stop_app_timer(sys_status_timer_id);
        break;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ��ȡ״̬��ģʽ�ȡ�
 * \param[in]    none
 * \param[out]   mode ����״̬��ģʽ���� headbar_mode_e  ö�����Ͷ���
 * \param[out]   icon_id ����Ӧ��ͼ��ID���ο� headbar_icon_e ö�����Ͷ���
 * \return       none
 * \ingroup      controls
 * \note
 *******************************************************************************/
void gui_get_headbar_mode(headbar_mode_e *mode, headbar_icon_e *icon_id)
{
    *mode = this_headbar_mode;
    *icon_id = this_headbar_icon_id;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ״̬�����»���ִ�нӿڡ�
 * \param[in]    update ״̬������ģʽ���� headbar_update_e ����
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_NULL û�κ�����
 * \ingroup      controls
 * \note
 * \li  �ýӿ��� common ģ���Զ����ã��û������Ԥ��
 * \li  ����û��Լ���Ҫ����ʲôʱ����ʲô��ʽˢ�£�Ҳ�����Լ��������á�
 *******************************************************************************/
app_result_e gui_headbar(headbar_update_e update)
{
    picbox_private_t headbar_pic;
    style_infor_t style_infor;

    //��ʱ����
    time_t temp_sys_time;
    uint8 temp_cable_state;
    uint8 temp_card_state;
    uint8 temp_icon_id;
    uint8 real_battery_value;
    if (this_headbar_style.bg_sty != V_U16_INVALID)
    {
        if ((update == HEADBAR_UPDATE_INIT) || (update == HEADBAR_UPDATE_ALL))
        {
            //ˢ�±���ͼƬ
            style_infor.style_id = this_headbar_style.bg_sty;
            style_infor.type = UI_COM;
            ui_show_picbox(&style_infor, NULL);
        }
    }

    if (this_headbar_style.battery_sty != V_U16_INVALID)
    {
        if (update == HEADBAR_UPDATE_INIT)
        {
            g_app_info_state.charge_state = (uint8) (int) key_chargeget(&real_battery_value);
            g_app_info_state.bat_value = real_battery_value;

            if (g_app_info_state.cable_state == CABLE_STATE_CABLE_NULL)//USB����û�в���
            {
                g_app_info_state.charge_state = BAT_NORMAL;
            }

            _HEADBAR_PRINTD_INFO_INT("bat_001",real_battery_value);
            g_battery_display = 1;
        }

        //���ڳ��
        if (g_app_info_state.charge_state == BAT_CHARGING)
        {
            if (g_battery_display > BATTERY_GRADE_MAX)
            {
                g_battery_display = 1;
            }_HEADBAR_PRINTD_INFO_INT("bat_002",g_battery_display);
            __show_battery(g_battery_display);
            g_battery_value = -1;//������Чֵ��һ���л�����ع��磬��������ͼ��
        }
        //�������
        else if (g_app_info_state.charge_state == BAT_FULL)
        {
            _HEADBAR_PRINTD_INFO_INT("bat_003",g_battery_display);
            __show_battery(BATTERY_GRADE_MAX);
            g_battery_value = -1;//������Чֵ��һ���л�����ع��磬��������ͼ��
        }
        //�޳�磬��ع���
        else
        {
            if ((update == HEADBAR_UPDATE_INIT) || (update == HEADBAR_UPDATE_ALL) || (g_app_info_state.bat_value
                    != g_battery_value))
            {

                _HEADBAR_PRINTD_INFO_INT("bat_004",g_app_info_state.bat_value);
                __show_battery(g_app_info_state.bat_value);
                g_battery_value = g_app_info_state.bat_value;
            }
        }
    }

    if (this_headbar_style.icon_sty != V_U16_INVALID)
    {
        if (g_app_info_state.keylock_state == TRUE)
        {
            temp_icon_id = HEADBAR_ICON_KEYLOCK;
        }
        else
        {
            temp_icon_id = this_headbar_icon_id;
        }

        if ((update == HEADBAR_UPDATE_INIT) || (update == HEADBAR_UPDATE_ALL) || ((temp_icon_id != HEADBAR_ICON_NONE)
                && (temp_icon_id != g_icon_id)))
        {
            //ˢ��ͼ��
            style_infor.style_id = this_headbar_style.icon_sty;
            style_infor.type = UI_COM;
            headbar_pic.pic_id = V_U16_INVALID;
            headbar_pic.frame_id = (uint8) temp_icon_id;
            ui_show_picbox(&style_infor, &headbar_pic);

            g_icon_id = temp_icon_id;
        }
    }

    if (this_headbar_style.time_sty != V_U16_INVALID)
    {
        timebox_draw_mode_e draw_mode;
        bool need_draw = FALSE;

        //��ȡ��ǰʱ��
        sys_get_time(&temp_sys_time);

        if ((update == HEADBAR_UPDATE_INIT) || (update == HEADBAR_UPDATE_ALL))
        {
            draw_mode = TIMEBOX_DRAW_ALL;//ʱ���ʼ��
            need_draw = TRUE;
        }
        else if (temp_sys_time.minute != g_sys_minute)
        {
            draw_mode = TIMEBOX_DRAW_TIME;//����ʱ��
            need_draw = TRUE;
        }
        else
        {
            //do nothing for QAC
        }

        //��ʾϵͳʱ��
        if (need_draw == TRUE)
        {
            timebox_private_t headbar_time;
            time_timebox_t *tmp_time;

            tmp_time = &(headbar_time.times[0].union_time.time);
            tmp_time->hour = temp_sys_time.hour;
            tmp_time->minute = temp_sys_time.minute;
            tmp_time->second = temp_sys_time.second;
            headbar_time.times[0].time_type = 'T';
            headbar_time.times[0].mode = TIME_DISP_HHMM;

            style_infor.style_id = this_headbar_style.time_sty;
            style_infor.type = UI_COM;
            ui_show_timebox(&style_infor, &headbar_time, (uint32) draw_mode);
            g_sys_minute = temp_sys_time.minute;
        }
    }

    if (this_headbar_style.cable_sty != V_U16_INVALID)
    {
        temp_cable_state = get_cable_state();

        if ((update == HEADBAR_UPDATE_INIT) || (update == HEADBAR_UPDATE_ALL) || (temp_cable_state
                != g_app_last_state.last_cable_state))
        {
            //ˢ�µ���״̬ͼ��
            style_infor.style_id = this_headbar_style.cable_sty;
            style_infor.type = UI_COM;
            headbar_pic.pic_id = V_U16_INVALID;
            headbar_pic.frame_id = temp_cable_state;
            ui_show_picbox(&style_infor, &headbar_pic);

            g_app_last_state.last_cable_state = temp_cable_state;
        }
    }

    if (this_headbar_style.card_sty != V_U16_INVALID)
    {
        temp_card_state = get_card_state();

        if ((update == HEADBAR_UPDATE_INIT) || (update == HEADBAR_UPDATE_ALL) || (temp_card_state
                != g_app_last_state.last_card_state))
        {
            //ˢ�¿�״̬ͼ��
            style_infor.style_id = this_headbar_style.card_sty;
            style_infor.type = UI_COM;
            headbar_pic.pic_id = V_U16_INVALID;
            headbar_pic.frame_id = temp_card_state;
            ui_show_picbox(&style_infor, &headbar_pic);

            g_app_last_state.last_card_state = temp_card_state;
        }
    }

    return RESULT_NULL;
}

/*! \endcond */

/*! \cond */

//���µ��ͼ��
static void __show_battery(uint8 display)
{
    picbox_private_t headbar_pic;
    style_infor_t style_infor;

    style_infor.style_id = this_headbar_style.battery_sty;
    style_infor.type = UI_COM;
    headbar_pic.pic_id = V_U16_INVALID;
    headbar_pic.frame_id = display;
    ui_show_picbox(&style_infor, &headbar_pic);
}

/*! \endcond */
