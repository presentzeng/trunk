/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-volumebar
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-15 9:51     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_volumebar.c
 * \brief    commonUI �������ÿؼ�������ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ����������GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�parambox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2011-9-15
 *******************************************************************************/

#include "common_ui.h"
#include "dac_interface.h"


extern void show_volumebar_ui(style_infor_t *style_infor, parambox_private_t *private_data,
        uint8 mode, progress_draw_mode_e progress_draw_mode) __FAR__;

void timeup_func_proc(void)
{
    g_vol_time_up_flag = TRUE;
}

//volumebar gui app msg deal
app_result_e volumebar_msg_deal(param_com_data_t *param_com, int8 timeup_id, uint8 mode, progress_draw_mode_e* pprogress_draw_mode)
{
    parambox_private_t *private_data;
    parambox_one_t *cur_one;
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    app_result_e result = RESULT_NULL;
    const key_map_t *this_key_map_list;

    private_data = param_com->private_data;
    cur_one = &(private_data->items[private_data->active]);

    //��ȡgui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)//��gui��Ϣ
    {
        //�а��������¼��㳬ʱʱ��
        if (mode == SET_VOLUME_VALUE)
        {
            restart_app_timer(timeup_id);
        }

        if (mode == SET_VOLUME_VALUE)
        {
            this_key_map_list = volume_key_map_list;
        }
        else
        {
            this_key_map_list = limit_key_map_list;
        }

        //���� gui��Ϣӳ��
        if (com_key_mapping(&input_msg, &gui_event, this_key_map_list) == TRUE)
        {
            switch (gui_event)
            {
                case EVENT_VOLUME_INC:
                //����ֵ����
                if (mode == SET_VOLUME_LIMIT)
                {
                    if (cur_one->value < cur_one->max)
                    {
                        cur_one->value += cur_one->step;
                        param_com->draw_mode = PARAMBOX_DRAW_VALUE;
			libc_print("myprint again", 2, 2);
			
			dac_set_volume(5 * (cur_one->value));
                        com_set_sound_volume((uint8) (cur_one->value));
                    }

                    break;//for QAC������case��֧����
                }

                //���ˣ�˵��mode������SET_VOLUME_LIMIT ��for QAC ������1�����Ƕ��
                if (cur_one->value < sys_comval->volume_limit)
                {
                    cur_one->value += cur_one->step;
                    param_com->draw_mode = PARAMBOX_DRAW_VALUE;

                    if (mode == SET_VOLUME_VALUE)
                    {
                    	
                    	dac_set_volume(5*(cur_one->value));
                        com_set_sound_volume((uint8) (cur_one->value));
                    }
                }
                break;

                case EVENT_VOLUME_DEC:
                //����ֵ�ݼ�
                if (cur_one->value > cur_one->min)
                {
                    cur_one->value -= cur_one->step;
                    param_com->draw_mode = PARAMBOX_DRAW_VALUE;

                    if ((mode == SET_VOLUME_LIMIT) || (mode == SET_VOLUME_VALUE))
                    {
                    	
                    	dac_set_volume(5*(cur_one->value));
                        com_set_sound_volume((uint8) (cur_one->value));
                    }
                }
                break;

                case EVENT_VOLUME_CONFIRM:
                //ȷ�����˳���������
                result = RESULT_CONFIRM;
                break;

                case EVENT_VOLUME_CANCEL:
                //ȡ�����˳���������
                result = RESULT_REDRAW;
                break;

                default:
                result = com_message_box(gui_event);
                if (result == RESULT_REDRAW)
                {
                    param_com->draw_mode = PARAMBOX_DRAW_ALL;
                    *pprogress_draw_mode = PROGRESS_DRAW_ALL;
                    volume_enter_forbidden = 0xff;
                    result = RESULT_NULL;//���˳��ؼ�
                }
                break;
            }
        }
    }
    else
    {
        //���ˣ�gui��Ϣ�����Ѿ��������
        //����ap˽����Ϣ��ϵͳ��Ϣ
        if (get_app_msg(&private_msg) == TRUE)
        {
            result = g_this_app_msg_dispatch(&private_msg);
            if (result == RESULT_REDRAW)
            {
                param_com->draw_mode = PARAMBOX_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
            }
        }
    }

    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �������ÿؼ����������������ǲ������ÿؼ����ⲿ�ӿ�
 * \param[in]    style_id ui editor������ƵĲ������ÿؼ�ģ��
 * \param[in]    param_com ָ��������ÿؼ���ʼ���ṹ����������ϲ�Բ������ÿؼ��Ŀ�����������
 * \param[in]    mode ����ģʽ�� SET_VOLUME_VALUE ��ʾ����ʵ�������� SET_VOLUME_LIMIT
 *               ��ʾ������������ֵ��SET_VOLUME_ALARM ר���� alarm Ӧ������ alarm ����
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_CONFIRM ȷ���������÷���
 * \retval           RESULT_REDRAW ȡ�����������˳��������������ƽ�������������������ʱ 4 ����û
 *                                  �������Զ����� RESULT_REDRAW��
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \note
 *******************************************************************************/
app_result_e ui_set_parameter_volume(uint16 style_id, param_com_data_t *param_com, uint8 mode)
{
    parambox_private_t *private_data;
    style_infor_t style_infor;
    app_result_e result;
    int8 timeup_id;//����������ʱ���ض�ʱ��
    progress_draw_mode_e progress_draw_mode;

    private_data = param_com->private_data;
    style_infor.style_id = style_id;
    style_infor.type = UI_COM;

    //����󣬻�ȡĬ�ϲ���
    private_data->old = private_data->active = 0;
    volume_value_last = private_data->items[private_data->active].value;//�����ݼ�ǰ����ֵΪ last ֵ

    if (mode == SET_VOLUME_VALUE)
    {
        //���˵���ǰ����������������ֻ֤��Ӧһ����������
        com_filter_key_hold();

        //���� 4 �붨ʱ��
        g_vol_time_up_flag = FALSE;
        timeup_id = set_app_timer(APP_TIMER_ATTRB_UI, 4000, timeup_func_proc);
    }

    //��ʼ��
    param_com->draw_mode = PARAMBOX_DRAW_ALL;
    volume_enter_forbidden = 0xff;
    progress_draw_mode = PROGRESS_DRAW_ALL;

    //�������ò�����Ϣѭ��
    while (1)
    {
        //����������ʱ����
        if ((mode == SET_VOLUME_VALUE) && (g_vol_time_up_flag == TRUE))
        {
            result = RESULT_REDRAW;
            break;
        }

        if (param_com->draw_mode != PARAMBOX_DRAW_NULL)
        {
            //��ʾ������UI
            show_volumebar_ui(&style_infor, private_data, mode, progress_draw_mode);
            //����lastֵ
            volume_value_last = private_data->items[private_data->active].value;
            param_com->draw_mode = PARAMBOX_DRAW_NULL;
            progress_draw_mode = PROGRESS_DRAW_PROGRESS;
        }

        result = volumebar_msg_deal(param_com, timeup_id, mode, &progress_draw_mode);
        if (result != RESULT_NULL)
        {
            break;
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    if (mode == SET_VOLUME_VALUE)
    {
        kill_app_timer(timeup_id);
    }
    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �������ÿؼ�����������ֵ����������ֵ��
 * \param[in]    volume ��ǰ����ֵָ��
 * \param[in]    limit ��������ֵָ��
 * \param[in]    mode ����ģʽ�� SET_VOLUME_VALUE ��ʾ����ʵ�������� SET_VOLUME_LIMIT
 *               ��ʾ������������ֵ��SET_VOLUME_ALARM ר���� alarm Ӧ������ alarm ����
 * \param[out]   volume �������úõ�����ֵ
 * \return       app_result_e��ͬ ui_set_parameter_volume һ�£�
 * \retval           RESULT_CONFIRM ȷ�����������ƣ����÷���
 * \retval           RESULT_REDRAW ȡ�����������˳��������������ƽ���������
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \note
 *******************************************************************************/
app_result_e gui_volumebar(uint8 *volume, uint8 *limit, uint8 mode)
{
    param_com_data_t param_com;
    parambox_private_t param_private;
    parambox_one_t param_volume;
    app_result_e result;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;
    //gui_direct_e direct_bk;

    //draw bg
    ui_set_backgd_color(Color_BACKCOLOR_def);//��Ϊȫ����
    ui_clear_screen(NULL);

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_VOLUME);

    //direct_bk = com_get_gui_direction();

    //com_set_gui_direction(GUI_DIRECT_NORMAL);

    gui_get_headbar_mode(&mode_backup, &icon_backup);
    if (mode == SET_VOLUME_VALUE)
    {
        gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);
    }
    else//�����������ã�ͬ��ͨ��������һ��
    {
        gui_set_headbar_mode(HEADBAR_MODE_NORMAL, this_headbar_icon_id);
    }

    param_volume.unit_id = V_U16_INVALID;
    param_volume.min = 0;
    param_volume.max = VOLUME_VALUE_MAX;
    param_volume.step = 1;

    if ((int16) (*limit) < (int16) (*volume))
    {
        (int16) (*volume) = (int16) (*limit);
    }

    if (mode == SET_VOLUME_LIMIT)
    {
        param_volume.value = (int16) (*limit);
    }
    else
    {
        param_volume.value = (int16) (*volume);
    }
    param_volume.value_str = NULL;
    param_volume.cycle = FALSE;
    param_volume.max_number = 2;
    param_volume.adjust_func = NULL;

    param_private.back_id = -1;
    param_private.icon_id = V_U16_INVALID;
    param_private.title_id = V_U16_INVALID;
    param_private.param_cnt = 1;
    param_private.active = 0;
    param_private.old = 0;
    param_private.items = &param_volume;
    param_private.sign_flag = 0;

    param_com.private_data = &param_private;

    result = ui_set_parameter_volume(VOLUMEPROGRESSBAR, &param_com, mode);
    if (mode == SET_VOLUME_VALUE)
    {
        *volume = (uint8) param_volume.value;
    }
    else if (mode == SET_VOLUME_ALARM)
    {
        if (result == RESULT_CONFIRM)
        {
            *volume = (uint8) param_volume.value;
        }
    }
    else//��������
    {
        if (result == RESULT_CONFIRM)
        {
            *limit = (uint8) param_volume.value;
            if (*volume > (uint8) param_volume.value)
            {
                *volume = (uint8) param_volume.value;
            }
            else
            {
                com_set_sound_volume(*volume);
            }
        }
        else
        {
            com_set_sound_volume(*volume);
        }
    }

    //com_set_gui_direction(direct_bk);
    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}
