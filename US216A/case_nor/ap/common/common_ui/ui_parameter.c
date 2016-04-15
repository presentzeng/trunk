/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-14 20:29     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_parameter.c
 * \brief    gui ���������������ÿؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�֣��ࣩ�������ÿؼ���
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��ui ������applib���ȵȡ�
 * \version  1.0
 * \date     2011-9-14
 *******************************************************************************/

#include "common_ui.h"
#include "display.h"

/*! \cond COMMON_API */

//parambox value inc
void parambox_value_inc(param_com_data_t *param_com)
{
    parambox_private_t *private_data;
    parambox_one_t *cur_one;

    private_data = param_com->private_data;
    cur_one = &(private_data->items[private_data->active]);

    if (cur_one->value < cur_one->max)
    {
        cur_one->value += cur_one->step;
        //��ʱ�ص�����
        if (cur_one->callback != NULL)
        {
            cur_one->callback(cur_one->value);
        }
        param_com->draw_mode = PARAMBOX_DRAW_VALUE;
    }
    else if (cur_one->cycle == TRUE)//����ѭ�����ڲ���
    {
        cur_one->value = cur_one->min;
        //��ʱ�ص�����
        if (cur_one->callback != NULL)
        {
            cur_one->callback(cur_one->value);
        }
        param_com->draw_mode = PARAMBOX_DRAW_VALUE;
    }
    else
    {
        //do nothing for QAC
    }

    if (param_com->draw_mode == PARAMBOX_DRAW_VALUE)
    {
        //����ֵ���;���
        if (cur_one->adjust_func != NULL)
        {
            adjust_result_e adjust_result = ADJUST_RESULT_NULL;

            adjust_result = cur_one->adjust_func(private_data->items, private_data->active, TRUE);
            if (adjust_result == ADJUST_RESULT_ALL)
            {
                param_com->draw_mode = PARAMBOX_DRAW_ALL;
            }
        }
    }
}

//parambox value dec
void parambox_value_dec(param_com_data_t *param_com)
{
    parambox_private_t *private_data;
    parambox_one_t *cur_one;

    private_data = param_com->private_data;
    cur_one = &(private_data->items[private_data->active]);

    if (cur_one->value > cur_one->min)
    {
        cur_one->value -= cur_one->step;
        //��ʱ�ص�����
        if (cur_one->callback != NULL)
        {
            cur_one->callback(cur_one->value);
        }
        param_com->draw_mode = PARAMBOX_DRAW_VALUE;
    }
    else if (cur_one->cycle == TRUE)//����ѭ�����ڲ���
    {
        cur_one->value = cur_one->max;
        //��ʱ�ص�����
        if (cur_one->callback != NULL)
        {
            cur_one->callback(cur_one->value);
        }
        param_com->draw_mode = PARAMBOX_DRAW_VALUE;
    }
    else
    {
        //do nothing for QAC
    }

    if (param_com->draw_mode == PARAMBOX_DRAW_VALUE)
    {
        //����ֵ���;���
        if (cur_one->adjust_func != NULL)
        {
            adjust_result_e adjust_result = ADJUST_RESULT_NULL;

            adjust_result = cur_one->adjust_func(private_data->items, private_data->active, FALSE);
            if (adjust_result == ADJUST_RESULT_ALL)
            {
                param_com->draw_mode = PARAMBOX_DRAW_ALL;
            }
        }
    }
}

//parambox gui app msg deal
app_result_e parambox_msg_deal(param_com_data_t *param_com, progress_draw_mode_e *pprogress_draw_mode)
{
    parambox_private_t *private_data;
    parambox_one_t *cur_one;
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    app_result_e result = RESULT_NULL;

    private_data = param_com->private_data;
    cur_one = &(private_data->items[private_data->active]);

    //��ȡgui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)//��gui��Ϣ
    {
        //���а���ӳ��
        if (com_key_mapping(&input_msg, &gui_event, parameter_key_map_list) == TRUE)
        {
            //GUI������Ϣ����
            switch (gui_event)
            {
                case EVENT_PARAMETER_VALUE_INC:
                //����ֵ����
                parambox_value_inc(param_com);
                break;

                case EVENT_PARAMETER_VALUE_DEC:
                //����ֵ�ݼ�
                parambox_value_dec(param_com);
                break;

                //�л�����һ������
                case EVENT_PARAMETER_SELECT_NEXT:
                if (private_data->param_cnt > 1)
                {
                    private_data->old = private_data->active;
                    if (private_data->active == (private_data->param_cnt - 1))
                    {
                        private_data->active = 0;
                    }
                    else
                    {
                        private_data->active++;
                    }

                    param_com->draw_mode = PARAMBOX_DRAW_PARAM;
                }
                break;

                //�л�����һ������
                case EVENT_PARAMETER_SELECT_PREV:
                if (private_data->param_cnt > 1)
                {
                    private_data->old = private_data->active;
                    if (private_data->active == 0)
                    {
                        private_data->active = private_data->param_cnt - 1;
                    }
                    else
                    {
                        private_data->active--;
                    }

                    param_com->draw_mode = PARAMBOX_DRAW_PARAM;
                }
                break;

                case EVENT_PARAMETER_CONFIRM:
                //ȷ��ѡ�����
                result = RESULT_CONFIRM;
                break;

                case EVENT_PARAMETER_CANCEL:
                //ȡ���˳�
                result = RESULT_REDRAW;
                break;

                default:
                result = com_message_box(gui_event);
                if (result == RESULT_REDRAW)
                {
                    param_com->draw_mode = PARAMBOX_DRAW_ALL;
                    *pprogress_draw_mode = PROGRESS_DRAW_ALL;
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
                *pprogress_draw_mode = PROGRESS_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
            }
        }
    }

    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �������ÿؼ�������ʵ�֡�
 * \param[in]    style_id ui editor������ƵĲ������ÿؼ�ģ��
 * \param[in]    param_com ָ��������ÿؼ���ʼ���ṹ����������ϲ�Բ������ÿؼ��Ŀ�����������
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_CONFIRM ȷ���������÷���
 * \retval           RESULT_REDRAW ����������Ч����AP���ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \ingroup      controls
 * \par          exmaple code
 * \code
 param_com_data_t param_com;
 parambox_private_t private_data;
 parambox_one_t param_item;
 app_result_e result;

 param_item.unit_id = V_U16_INVALID;
 param_item.min = 0;
 param_item.max = 40;
 param_item.step = 1;
 param_item.value = 24;
 param_item.cycle = FALSE;
 param_item.max_number = 2;
 param_item.value_str = NULL;
 param_item.adjust_func = NULL;
 param_item.callback = volumn_cb;

 private_data.back_id = V_U16_INVALID;
 private_data.icon_id = P_SPEAKER;
 private_data.title_id = V_U16_INVALID;
 private_data.param_cnt = 1;
 private_data.items = &param_item;
 private_data.active = 0;
 private_data.sign_flag = 0;
 private_data.scale = 0;//��sliderģ�壬�������

 param_com.private_data = &private_data;
 result = gui_set_parameter(PARAMETER_SIMPLE, &param_com);//��ͨ����������
 if(result == RESULT_CONFIRM)//ȷ�����ò�������
 {

 }
 else if(result == RESULT_REDRAW)//ȡ�����ò������أ��ػ�UI
 {

 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 * \li  ͨ������� adjust_func ����Ҫ�õ����������������ʱ��Ҫ������������Լ��
 *      ����ֵ��������Ҫ��ʾΪ�ַ���ʱ�������øûص��������д���
 *******************************************************************************/
app_result_e gui_set_parameter(uint16 style_id, param_com_data_t *param_com)
{
    parambox_private_t *private_data;
    style_infor_t style_infor;
    style_infor_t temp_style_infor;
    app_result_e result = RESULT_NULL;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;

    progressbar_private_t vol_progress;
    progress_draw_mode_e progress_draw_mode;
    textbox_private_t textbox;
    uint8 cur_value_str_arr[3] =
    { 0 };

    //draw bg
    ui_set_backgd_color(Color_BACKCOLOR_def);//��Ϊȫ����
    ui_clear_screen(NULL);

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_PARAM);

    gui_get_headbar_mode(&mode_backup, &icon_backup);

    gui_set_headbar_mode(HEADBAR_MODE_NORMAL, this_headbar_icon_id);

    progress_draw_mode = PROGRESS_DRAW_ALL;
    param_com->draw_mode = PARAMBOX_DRAW_ALL;
    private_data = param_com->private_data;
    //����󣬻�ȡĬ�ϲ���
    if (private_data->active >= private_data->param_cnt)
    {
        private_data->active = 0;
    }
    private_data->old = private_data->active;

    style_infor.type = UI_COM;
    temp_style_infor.type = UI_COM;

    style_infor.style_id = style_id;

    //�������ò�����Ϣѭ��
    while (1)
    {
        if (param_com->draw_mode != PARAMBOX_DRAW_NULL)
        {
            //ǰ4bit��ʾ�ؼ����ͣ�0x07��ʾ parambox��else��ʾ progressbar
            if ((style_id & 0xf000) == 0x7000)
            {
                ui_show_parambox(&style_infor, private_data, (uint32) param_com->draw_mode);
            }
            else 
            {
                if (PROGRESS_DRAW_ALL == progress_draw_mode)
                {
                    //��ʼ������, ��ʾ���Ҽ�ͷ
                    temp_style_infor.style_id = PROGRESSBAR_LEFT;
                    ui_show_picbox(&temp_style_infor, NULL);
                    temp_style_infor.style_id = PROGRESSBAR_RIGHT;
                    ui_show_picbox(&temp_style_infor, NULL);

                    //��ʾ��λ
                    if (private_data->items->unit_id == 0xffff) //������ʾ����ͼ��
                    {
                        temp_style_infor.style_id = PIC_LIGHT_ICON; //��������ж������ȵ��ڣ���ʾ����ICON
                        ui_show_picbox(&temp_style_infor, NULL);
                    }
                    else //��ʱĬ����ʾʱ�䵥λ:��
                    {
                        temp_style_infor.style_id = TEXT_MIN;
                        textbox.lang_id = ENGLISH;
                        textbox.str_value = NULL;
                        textbox.str_id = private_data->items->unit_id;
                        ui_show_textbox((void*) &temp_style_infor, (void*) &textbox, TEXTBOX_DRAW_NORMAL);
                    }
                }

                //ui_show_sliderbar(&style_infor, private_data, (uint32)param_com->draw_mode);
                //ui_show_sliderbar(style_infor, private_data, (uint32)draw_mode);
                vol_progress.value = private_data->items->value;
                vol_progress.total = private_data->items->max;
                ui_show_progressbar(&style_infor, &vol_progress, progress_draw_mode);

                //��ʾ��ǰֵ
                if (private_data->items->unit_id != 0xffff)
                {
                    libc_itoa(vol_progress.value, cur_value_str_arr, 2);
                    temp_style_infor.style_id = CURRENT_VALUE;
                    textbox.lang_id = ANSIDATAAUTO;
                    textbox.str_value = cur_value_str_arr;
                    textbox.str_id = -1;
                    ui_show_textbox((void*) &temp_style_infor, (void*) &textbox, TEXTBOX_DRAW_NORMAL);
                }
            }
            
            param_com->draw_mode = PARAMBOX_DRAW_NULL;
            progress_draw_mode = PROGRESS_DRAW_PROGRESS;
        }

        result = parambox_msg_deal(param_com, &progress_draw_mode);
        if (result != RESULT_NULL)
        {
            break;
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}

/*! \endcond */
