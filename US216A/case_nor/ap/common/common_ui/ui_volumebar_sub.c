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
 * \file     _ui_volumebar_sub.c
 * \brief    commonUI �������ÿؼ�������ʵ��--UI��ʾ����
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ����������GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�parambox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2012-3-8
 *******************************************************************************/

#include "common_ui.h"

//��ʾ���������˽������������Сͼ��
void show_volume_extra(uint16 style_id, uint16 pic_id, uint16 offset)
{
    style_infor_t temp_style_infor;
    region_t pic_region;
    uint16 x, y;

    temp_style_infor.style_id = style_id;
    temp_style_infor.type = UI_COM;
    ui_get_picbox_attrb(&temp_style_infor, &pic_region, 0);

    /*x = pic_region.x;
     y = pic_region.y - offset;*/
    x = pic_region.x + offset;
    y = pic_region.y;
    ui_show_pic(pic_id, x, y);
}

//��ʾ������UI
void show_volumebar_ui(style_infor_t *style_infor, parambox_private_t *private_data, uint8 mode,
        progress_draw_mode_e progress_draw_mode)
{
    parambox_one_t *cur_one = &(private_data->items[private_data->active]);
    uint16 total = (uint16) (cur_one->max - cur_one->min);
    uint16 current = (uint16) (cur_one->value - cur_one->min);
    //uint16 last_v = (uint16)(value_last - cur_one->min);
    uint16 limit = (uint16) (sys_comval->volume_limit - cur_one->min);
    volume_processbar_param attrb_data;
    style_infor_t temp_style_infor;
    progressbar_private_t vol_progress;
    textbox_private_t textbox_data;
    picbox_private_t volumebar_speaker_pic;
    uint8 cur_value_str_arr[3] =
    { 0 };

    temp_style_infor.type = UI_COM;

    if (volume_enter_forbidden == 0xff)//��ʼ������
    {
        //��ʾ���Ҽ�ͷ
        temp_style_infor.style_id = VOLUMEBAR_LEFT;
        ui_show_picbox(&temp_style_infor, NULL);
        temp_style_infor.style_id = VOLUMEBAR_RIGHT;
        ui_show_picbox(&temp_style_infor, NULL);
    }

    //��ȡprocessbar UI����
    temp_style_infor.style_id = VOLUME_PROCESSBAR_PARAM;
    //temp_style_infor.type = UI_COM;
    ui_load_attributebox(&temp_style_infor, &attrb_data, sizeof(volume_processbar_param));

    //����ɺۼ�
    // show_volume_extra(VOLUMEBAR_CURRENT_CLEAR, P_VOLUME_CURRENT_CLEAR, (last_v * attrb_data.slider_length) / total);

    //��ʾ���Ⱥ�����ֵ
    if (((volume_enter_forbidden == 0xff) //��ʼ������
            && (((mode != SET_VOLUME_LIMIT) && (cur_one->value < sys_comval->volume_limit)) || ((mode
                    == SET_VOLUME_LIMIT) && (cur_one->value < cur_one->max)))) || (((mode != SET_VOLUME_LIMIT)
            && (volume_enter_forbidden == 1) && (cur_one->value < sys_comval->volume_limit)) || ((mode
            == SET_VOLUME_LIMIT) && (volume_enter_forbidden == 1) && (cur_one->value < cur_one->max))))
    {
        /*temp_style_infor.style_id = VOLUMEBAR_SILENT_CLEAR;
         temp_style_infor.type = UI_COM;
         ui_show_picbox(&temp_style_infor, NULL);*/

        temp_style_infor.style_id = VOLUMEBAR_SPEAKER;
        volumebar_speaker_pic.pic_id = V_U16_INVALID;
        volumebar_speaker_pic.frame_id = 0;
        ui_show_picbox(&temp_style_infor, &volumebar_speaker_pic);
        volume_enter_forbidden = 0;
    }

    libc_itoa(current, cur_value_str_arr, 2);
    temp_style_infor.style_id = CURRENT_VOLUME_VALUE;
    //temp_style_infor.type = UI_COM;
    textbox_data.lang_id = ANSIDATAAUTO;
    textbox_data.str_value = cur_value_str_arr;
    textbox_data.str_id = -1;
    ui_show_textbox((void*) &temp_style_infor, (void*) &textbox_data, TEXTBOX_DRAW_NORMAL);

    //��ʾ������
    //ui_show_sliderbar(style_infor, private_data, (uint32)draw_mode);
    vol_progress.value = current;
    vol_progress.total = total;
    ui_show_progressbar(style_infor, &vol_progress, progress_draw_mode);

    //��������UIϸ�ڻ��ƣ���������ʵ�ߺ�����
    //��ʾ����������
    //libc_print("lim",limit,2);//2015-11-14
    show_volume_extra(VOLUMEBAR_LIMIT, P_VOLUME_LIMIT_LINE, (limit * attrb_data.processbar_length) / total);
    //��ʾ��ǰֵ
    //show_volume_extra(VOLUMEBAR_CURRENT, P_VOLUME_CURRENT_LINE, (current * attrb_data.slider_length) / total);

    //��ʾ������ֹͼ��
    if (((volume_enter_forbidden == 0xff) //��ʼ������
            && (((mode != SET_VOLUME_LIMIT) && (cur_one->value >= sys_comval->volume_limit)) || ((mode
                    == SET_VOLUME_LIMIT) && (cur_one->value == cur_one->max)))) || (((mode != SET_VOLUME_LIMIT)
            && (volume_enter_forbidden == 0) && (cur_one->value >= sys_comval->volume_limit)) || ((mode
            == SET_VOLUME_LIMIT) && (volume_enter_forbidden == 0) && (cur_one->value == cur_one->max))))
    {
        /*temp_style_infor.style_id = VOLUMEBAR_SILENT;
         ui_show_picbox(&temp_style_infor, NULL);*/
        temp_style_infor.style_id = VOLUMEBAR_SPEAKER;
        volumebar_speaker_pic.pic_id = V_U16_INVALID;
        volumebar_speaker_pic.frame_id = 1;
        ui_show_picbox(&temp_style_infor, &volumebar_speaker_pic);

        temp_style_infor.style_id = CURRENT_VOLUME_VALUE;
        ui_show_textbox((void*) &temp_style_infor, NULL, TEXTBOX_DRAW_NORMAL);
        volume_enter_forbidden = 1;
    }
}
