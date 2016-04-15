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
 * \file     _ui_screensave.c
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

extern void draw_digit_clock(bool flag) __FAR__;
extern void digit_clock_handle(void) __FAR__;
extern void album_art_handle(void) __FAR__;
//extern void demo_mode_handle(void) __FAR__;
extern void play_albumart_save(region_t *region);
extern void play_albumart_save_init(void);
/******************************************************************************/
/*!
 * \par  Description:
 *    ������Ļ����ģʽ��
 * \param[in]    ss_mode ��Ļ����ģʽ����ϸ��screen_save_mode_e����
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_NULL ûʵ������
 * \note
 *******************************************************************************/
app_result_e gui_screen_save_enter(screen_save_mode_e ss_mode)
{
    style_infor_t style_infor;

    g_ss_tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_SCREEN);

    //g_ss_direct_bk = com_get_gui_direction();
    com_set_gui_direction(GUI_DIRECT_NORMAL);

    gui_get_headbar_mode(&g_ss_mode_backup, &g_ss_icon_backup);
    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //������Ļ����
    change_screensave_state(TRUE);

    //����ר��ͼƬģʽ����û�в������ֻ�����ͣ���ţ��Զ��л�������ʱ��ģʽ
    if ((ss_mode == SCREEN_ALBUM_ART) && (get_engine_type() != ENGINE_MUSIC))
    {
        ss_mode = SCREEN_DIGIT_CLOCK;
    }

    g_ss_mode = ss_mode;

    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //ִ��������ʼ�����ֲ���
    switch (g_ss_mode)
    {
        case SCREEN_DIGIT_CLOCK:
        //ˢ����ͼ
        /*style_infor.style_id = DIGIT_CLOCK_BG;
         style_infor.type = UI_COM;
         ui_show_picbox(&style_infor, NULL);*/
        ui_set_backgd_color(Color_BACKCOLOR_def);
        ui_clear_screen(NULL);

        //�Ȼ��Ƴ�ʼ����ʱ�ӣ���������ʱ�Ӷ�Ҫ����
        draw_digit_clock(TRUE);
        //��������Ϊ 5S Ӧ�ü���ʱ�������ڸ������ں�ʱ����ʾ
        g_ss_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 5000, digit_clock_handle);
        break;

        case SCREEN_ALBUM_ART:
        play_albumart_save_init();
        //��������Ϊ 5S Ӧ�ü���ʱ�������ڸ��� Album Art ͼƬ
        g_ss_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 5000, album_art_handle);
        break;

        case SCRREN_SCREEN_OFF:
        //�ص�����ƣ��ص�����ǰ��������Ч������
        ui_set_backgd_color(0x0000);//��Ϊȫ����
        ui_clear_screen(NULL);
        lcd_backlight_on_off(FALSE);
        lcd_standby_screen(FALSE);
        change_backlight_state(BACKLIGHT_STATE_OFF);

        //����Ƶ��Ϊ 3M
        //g_ss_clk_backup = (uint8)sys_adjust_clk(FREQ_3M, 0);
        //g_ss_clk_backup = sys_adjust_clk(3, 0);
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
        adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
        break;

        default:
        break;
    }

    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �˳���Ļ����ģʽ��
 * \param[in]    ss_mode ��Ļ����ģʽ����ϸ��screen_save_mode_e����
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_REDRAW �˳���������Ҫ�ػ�UI
 * \retval           RESULT_NULL �ӳ��˳����������账��
 * \note
 *******************************************************************************/
app_result_e gui_screen_save_exit(void)
{
    //�������رգ��򿪱���
    if (get_backlight_state() == BACKLIGHT_STATE_OFF)
    {
        //�ж��Ƿ���Ҫ�ص����ڲ��Ž���
        if ((g_ss_vip_event != TRUE)//������Ϊ�����ȼ��¼��˳���Ļ����
                && (g_ss_delay_flag == FALSE)//����delay�˳�����
                && (get_engine_state() == ENGINE_STATE_PLAYING)//��̨���治���ڲ���״̬
                && (get_app_state() == APP_STATE_NO_PLAY))//ǰ̨Ӧ�ò����ڲ��� UI ״̬
        {
            msg_apps_t msg;

            //����ǰAP���� back_to_playing �첽��Ϣ
            msg.type = MSG_APP_BACKTO_PLAYING;
            send_async_msg(g_this_app_info->app_id, &msg);

            g_ss_delay_flag = TRUE;//delay�˳�����
            return RESULT_NULL;
        }

        //�ָ�������ǰƵ��
        //sys_adjust_clk(g_ss_clk_backup, 0);
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
        adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL6, FREQ_NULL);
        //��������
        lcd_standby_screen(TRUE);
        lcd_backlight_on_off(TRUE);
        com_set_contrast(sys_comval->lightness);
        change_backlight_state(BACKLIGHT_STATE_NORMAL);
    }

    //ִ�������˳�����
    switch (g_ss_mode)
    {
        case SCREEN_DIGIT_CLOCK:
        case SCREEN_ALBUM_ART:
        case SCREEN_DEMO_MODE:
        //ɾ����ʱ��
        kill_app_timer(g_ss_timer_id);
        break;

        default:
        break;
    }

    //�˳���Ļ����
    change_screensave_state(FALSE);
    //com_set_gui_direction(g_ss_direct_bk);
    change_app_timer_tag(g_ss_tag_backup);
    gui_set_headbar_mode(g_ss_mode_backup, g_ss_icon_backup);
    g_ss_mode = SCREEN_NULL;
    g_ss_vip_event = FALSE;
    return RESULT_REDRAW;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void play_albumart_save(region_t pic_region)
 * \����ר������album art
 * \param[in]    void  para1
 * \param[in]
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \retval       none
 * \ingroup      ui_screensave.c
 * \note
 */
/*******************************************************************************/
void play_albumart_save(region_t *pic_region)
{
#if 0
    region_t tmp_region;
#ifndef PC
    uint8* save_buf = (uint8*) 0x9fc3fc00;
#else
    uint8* save_buf = GET_REAL_ADDR(0x9fc3fc00);
#endif
    uint32 pic_cnt;
    uint8 i, loop_cnt;

    loop_cnt = (uint8)(pic_region->height / 2);

    //��������ģʽ
    lcd_set_draw_mode(DRAW_MODE_H_DEF);
    //�贰,��ȡ����
    tmp_region.x = pic_region->x;
    tmp_region.y = pic_region->y;
    tmp_region.width = pic_region->width;
    tmp_region.height = 2;

    pic_cnt = pic_region->width * 2;

    for (i = 0; i < loop_cnt; i++)
    {
        //�贰
        lcd_set_window(&tmp_region);
        //��ȡGRAM����
        lcd_get_buff_data(save_buf, pic_cnt);
        sys_vm_write(save_buf, VM_ALBUMART_INFO + i * 512, 512);
        tmp_region.y += 2;
    }
#if 0//height Ϊż����û�����������Ϊ���������
    pic_cnt = (pic_region.height % 2) * pic_region.width;
    if (pic_cnt != 0)
    {
        //�贰
        lcd_set_window(&tmp_region);
        lcd_get_buff_data(save_buf, pic_cnt);
        sys_vm_write(save_buf, VM_ALBUMART_INFO + i * 512);
    }
#endif
#endif
}
/******************************************************************************/
/*!
 * \par  Description:
 * \void play_albumart_save_init(void)
 * \��ʼ������������ʾר�����
 * \param[in]    void  para1
 * \param[in]
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \retval       none
 * \ingroup      ui_screensave.c
 * \note
 */
/*******************************************************************************/
void play_albumart_save_init(void)
{
    if (g_com_paint_cb.func != NULL)
    {
        ui_clear_screen(NULL);
        g_com_paint_cb.func(g_com_paint_cb.arg);
    }

    //���� albumart �� VRAM ȥ���Ա�������ȡ
    play_albumart_save(&(g_com_paint_cb.arg->region));

    g_ss_album_art_phase = 0;

    //��ʼ��ʾ
    //album_art_handle();

}
