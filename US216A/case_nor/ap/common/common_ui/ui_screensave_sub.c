/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-screensave
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-19 17:24     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_screensave_sub.c
 * \brief    commonUI ��Ļ����������ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ����Ļ����GUI����������ʱ�ӣ�����ר��ͼƬ���ر���ƣ���ʾģʽ�ȡ�
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�picbox����ɿؼ���ʾ������
 * \version  1.0
 * \date     2011-9-19
 *******************************************************************************/

#include "common_ui.h"

const uint16 album_art_x[ALBUM_ART_MAX_COUNT] =
{ 0, 36, 0, 36, 0, 36 };
const uint16 album_art_y[ALBUM_ART_MAX_COUNT] =
{ 0, 34, 68, 0, 34, 68 };

/******************************************************************************/
/*!
 * \par  Description:
 *    ��������ʱ��
 * \param[in]    flag ����ʱ�Ӹ���ģʽ��true��ʾȫ�����£�false��ʾ���²���
 * \param[out]   none
 * \return       none
 * \note
 *******************************************************************************/
void draw_digit_clock(bool flag)
{
    date_t *p_sys_date;
    time_t tmp_sys_time;
    time_timebox_t *tmp_time;
    picbox_private_t ampm_pic;
    timebox_private_t ss_time;
    style_infor_t style_infor;
    timebox_draw_mode_e mode;
    uint8 tmp_format;

    if (flag == TRUE)
    {
        mode = TIMEBOX_DRAW_ALL;
    }
    else
    {
        mode = TIMEBOX_DRAW_TIME;
    }

    //��ȡϵͳ���ڲ�������ʾ
    p_sys_date = &ss_time.times[0].union_time.date;
    sys_get_date(p_sys_date);

    //ѡȡ��ʾ��ʽ
    if (sys_comval->date_format == 1)
    {
        tmp_format = DATE_DISP_MMDDYY;
    }
    else
    {
        tmp_format = DATE_DISP_YYMMDD;
    }

    ss_time.times[0].time_type = 'D';
    ss_time.times[0].mode = tmp_format;

    //ֻ�ڳ�ʼ���и���ʱ����ʾ
    if ((flag == TRUE) || (p_sys_date->day != g_ss_last_date))
    {
        if (p_sys_date->year >= 2099)
        {
            p_sys_date->year = 2000;
            sys_set_date(p_sys_date);
            sys_get_date(p_sys_date);
        }

        style_infor.style_id = DIGIT_CLOCK_DATE;
        style_infor.type = UI_COM;
        ui_show_timebox(&style_infor, &ss_time, (uint32) mode);
        g_ss_last_date = p_sys_date->day;
    }

    //��ȡϵͳʱ�䲢������ʾ
    sys_get_time(&tmp_sys_time);
    tmp_time = &(ss_time.times[0].union_time.time);
    tmp_time->hour = tmp_sys_time.hour;
    tmp_time->minute = tmp_sys_time.minute;
    tmp_time->second = tmp_sys_time.second;

    ss_time.times[0].time_type = 'T';
    ss_time.times[0].mode = TIME_DISP_HHMM;

    //ֻ�ڳ�ʼ���и���ʱ����ʾ
    if ((flag == TRUE) || (tmp_sys_time.minute != g_ss_last_time))
    {
        if (sys_comval->time_format == 1)
        {
            style_infor.style_id = DIGIT_CLOCK_TIME_24;
            style_infor.type = UI_COM;
            ui_show_timebox(&style_infor, &ss_time, (uint32) mode);
            g_ss_last_time = tmp_sys_time.minute;
        }
        else
        {
            style_infor.style_id = DIGIT_CLOCK_TIME_12;
            style_infor.type = UI_COM;
            ui_show_timebox(&style_infor, &ss_time, (uint32) mode);

            style_infor.style_id = DIGIT_CLOCK_TIME_AMPM;
            ampm_pic.pic_id = V_U16_INVALID;
            if (tmp_sys_time.hour < 12)
            {
                ampm_pic.frame_id = 0;//am
            }
            else
            {
                ampm_pic.frame_id = 1;//pm
            }
            //Сʱת��Ϊ12Сʱ��
            tmp_sys_time.hour = tmp_sys_time.hour % 12;
            if (tmp_sys_time.hour == 0)
            {
                tmp_sys_time.hour = 12;
            }
            ui_show_picbox(&style_infor, &ampm_pic);
            g_ss_last_time = tmp_sys_time.minute;
        }
    }
}

static void _trans_data_to_dma(uint32 width, uint32 size)
{
#if 0
    lcd_dma_set_counter(size / 2, width);
    lcd_dma_start_trans(LCD_RAM_INDEX);
#endif
}

static void show_album_art(uint8 index)
{
#if 0
    uint32 pic_size = 92 * 92 * 2;
    uint32 real_size;
    uint32 album_art_addr = VM_ALBUMART_INFO;
    uint8 *album_art_buf = (uint8 *)LCD_BUF_ADDR;
    region_t region;

    lcd_set_draw_mode(DRAW_MODE_H_DEF);

    region.x = album_art_x[index];
    region.y = album_art_y[index];
    region.width = 92;
    region.height = 92;
    lcd_set_window(&region);

    while(pic_size> 0)
    {
        if(pic_size> 92 * 4)
        {
            real_size = 92 * 4;
        }
        else
        {
            real_size = pic_size;
        }
        sys_vm_read(album_art_buf, album_art_addr, 512);
        _trans_data_to_dma(92, real_size);
        pic_size -= real_size;
        album_art_addr += 512;
    }
#endif
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ����ʱ����Ļ������ʱ���������̣��������ڸ�������ʱ��
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \note
 *******************************************************************************/
void digit_clock_handle(void)
{
    //��������ʱ��
    draw_digit_clock(FALSE);
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ����ר��ͼƬ��Ļ������ʱ���������̣����������л�����ר��
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \note
 *******************************************************************************/
void album_art_handle(void)
{
    style_infor_t style_infor;

    //ˢ����ͼ
    /*style_infor.style_id = ALBUM_ART_BG;
     style_infor.type = UI_COM;
     ui_show_picbox(&style_infor, NULL);*/
    ui_set_backgd_color(Color_BACKCOLOR_def);
    ui_clear_screen(NULL);

    //����music�л�������ר��ͼƬ�Ľӿ�
    show_album_art(g_ss_album_art_phase);
    g_ss_album_art_phase++;
    if (g_ss_album_art_phase >= ALBUM_ART_MAX_COUNT)
    {
        g_ss_album_art_phase = 0;
    }
}
