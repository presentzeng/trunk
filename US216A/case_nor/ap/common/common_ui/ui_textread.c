/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-textread
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-19 21:49     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_textread.c
 * \brief    ����ı�����ؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�ּ���ı���� GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��applib���ȵȡ�
 * \version  1.0
 * \date     2011-9-19
 *******************************************************************************/

#include "common_ui.h"

extern bool sd_fseek_sector(sd_handle file_handle, uint8 where, int32 sector_offset);
extern bool sd_fread_sector(sd_handle file_handle, uint8 *text_buf, uint32 sector_len);
extern void show_text_line(string_desc_t *string_infor, uint8 line);

#define HISTORY_MAX_PAGE    32//ֻ֧��32ҳ�ڵļ���ı��ļ�
//�ı������ʷ���壬��໺��16ҳ������������offsetֵ
uint16 g_offset_history[HISTORY_MAX_PAGE] _BANK_DATA_ATTR_;
uint16 g_file_offset _BANK_DATA_ATTR_;
uint16 g_file_offset_bk _BANK_DATA_ATTR_;
uint8 g_page_index _BANK_DATA_ATTR_;
bool g_text_end_flag _BANK_DATA_ATTR_;
bool g_need_draw _BANK_DATA_ATTR_;

//�ı��Ķ����뻺�����ṹ��ȫ�ֱ�����ui_textread_dec.c ����ʣ���Ҫ�л���ȥ
text_decode_t text_decode _BANK_DATA_ATTR_;

//�ı��Ķ������ļ��ṹ��ָ�룬ui_textread_dec.c ����ʣ���Ҫ�л���ȥ
text_file_t text_file _BANK_DATA_ATTR_;

//textread gui app msg deal
app_result_e textread_msg_deal(void)
{
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    app_result_e result = RESULT_NULL;

    //��ȡgui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)//��gui��Ϣ
    {
        //���а���ӳ��
        if (com_key_mapping(&input_msg, &gui_event, textread_key_map_list) == TRUE)
        {
            //GUI������Ϣ����
            switch (gui_event)
            {
                case EVENT_TEXTREAD_NEXT_PAGE:
                //���·�ҳ
                if (g_text_end_flag == TRUE)
                {
                    //�ѵ�ĩβ
                    break;
                }
                else
                {
                    g_page_index++;
                    g_need_draw = TRUE;
                }
                break;

                case EVENT_TEXTREAD_PREV_PAGE:
                //���Ϸ�ҳ
                if (g_page_index == 0)
                {
                    //�ѵ���ҳ
                    break;
                }
                else
                {
                    g_page_index--;
                    g_file_offset = g_offset_history[g_page_index];
                    g_need_draw = TRUE;

                    text_decode.valid = FALSE;
                }
                break;

                case EVENT_TEXTREAD_CANCEL:
                //�����ı��Ķ����˳�
                result = RESULT_REDRAW;
                break;

                default:
                result = com_message_box(gui_event);
                if (result == RESULT_REDRAW)
                {
                    g_need_draw = TRUE;
                    g_file_offset = g_file_offset_bk;//�ػ�
                    text_decode.valid = FALSE;
                    text_decode.remain = 0;
                    result = RESULT_NULL;//���˳��ؼ�
                }
                break;
            }
        }
    }
    else
    {
        //���ˣ�gui��Ϣ�����Ѿ��������
        //��ȡap˽����Ϣ��ϵͳ��Ϣ������ת��Ϊ˽����Ϣ�ٷ��أ���������
        if (get_app_msg(&private_msg) == TRUE)
        {
            //setting Ӧ��ֻ����ӦĬ�ϴ������Ϣ�������� com_message_box ����
            result = com_message_box(private_msg.msg.type);
            if (result == RESULT_REDRAW)
            {
                g_need_draw = TRUE;
                g_file_offset = g_file_offset_bk;//�ػ�
                text_decode.valid = FALSE;
                text_decode.remain = 0;
                result = RESULT_NULL;//���˳��ؼ�
            }
        }
    }

    return result;
}

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    ����ı�����ؼ�������ʵ�֣��ýӿ�ֻ֧�� SD �ļ�ϵͳ�е��ı��ļ���
 * \param[in]    style_id ui editor ������Ƶ��ı�����ؼ�ģ��
 * \param[in]    file_name �Ķ��ļ�����
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_REDRAW ��������AP���ػ�UI
 * \retval           RESULT_ERROR �޷����ļ��ȣ����ش��󣨳���ϵͳbug�����򲻿��ܷ��أ�
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \ingroup      controls
 * \par          exmaple code
 * \code
 ����1����ʾ��������˵����
 const char law_file_name[12] = "legal.txt";
 app_result_e result;

 result = gui_text_read(law_file_name);
 * \endcode
 * \note
 * \li  �ýӿ�ֻ֧�� SD �ļ�ϵͳ�е��ı��ļ��������ļ�ҳ�����ܳ��� 32 ҳ��
 * \li  ֧�� Unicode-16��UTF-8��������루�μ� lang_id.h �ж��������б���ʽ��
 *******************************************************************************/
app_result_e gui_text_read(const char *file_name)
{
    app_result_e result = RESULT_NULL;
    sd_handle sd_file_handle;
    uint32 sd_file_length;
    uint16 page_bytes;
    text_end_mode_e end_mode;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_TEXTRD);

    gui_get_headbar_mode(&mode_backup, &icon_backup);
    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //style_infor.style_id = style_id;
    //style_infor.type = UI_COM;

    //��ʼ���ı��ļ�������text_file_t���ݽṹ
    sd_file_handle = sys_sd_fopen(file_name);
    if (sd_file_handle == NULL)
    {
        return RESULT_ERROR;
    }

    sys_sd_fread(sd_file_handle, text_decode.text_buf, 512);
    if ((text_decode.text_buf[0] == 0xff) && (text_decode.text_buf[1] == 0xfe))
    {
        text_decode.param.language = UNICODEDATA;
    }
    else if ((text_decode.text_buf[0] == 0xef) && (text_decode.text_buf[1] == 0xbb)
            && (text_decode.text_buf[2] == 0xbf))
    {
        text_decode.param.language = UTF_8DATA;
    }
    else
    {
        text_decode.param.language = (int8) sys_comval->language_id;
    }

    sys_sd_fseek(sd_file_handle, SEEK_END, 0);
    sd_file_length = sys_sd_ftell(sd_file_handle);
    sys_sd_fseek(sd_file_handle, SEEK_SET, 0);
    sd_file_length = (sd_file_length - sys_sd_ftell(sd_file_handle) + 1);//��λָ�������1Ϊ�ļ�����

    text_file.file_handle = sd_file_handle;
    text_file.file_length = sd_file_length;
    text_file.file_sectors = sd_file_length / 512;
    if ((sd_file_length % 512) != 0)
    {
        text_file.file_sectors++;
    }
    text_file.fseek = sd_fseek_sector;
    text_file.fread = sd_fread_sector;

    //��ʼ����ʷ��¼
    libc_memset(g_offset_history, 0xff, sizeof(g_offset_history));
    g_file_offset = 0;
    g_file_offset_bk = 0;
    g_page_index = 0;
    g_text_end_flag = FALSE;
    g_need_draw = TRUE;

    text_file.file_offset = 0;
    text_decode.valid = FALSE;
    text_decode.remain = 0;

    text_decode.param.line_count_one_page = 10;
    text_decode.param.max_width_one_line = ROW_MAX_WIDTH;
    text_decode.param.mode = FILTER_BLANK_DISABLE | DIVIDE_WORD_ENABLE;

    text_decode.text_show_line = show_text_line;

    //gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    //�����ı����ѭ��
    while (1)
    {
        //��ʾһҳ�ı�
        if (g_need_draw == TRUE)
        {
            g_offset_history[g_page_index] = g_file_offset;
            //ui_show_picbox(&style_infor, NULL);
            //draw bg
            ui_set_backgd_color(Color_BACKCOLOR_def);
            ui_clear_screen(NULL); //2015-10-30

            text_file.file_offset = g_file_offset;
            g_file_offset_bk = g_file_offset;
            end_mode = text_decode_one_page(&text_decode, &text_file, &page_bytes);
            if (end_mode == TEXT_END_NUL)//��������������ֹ�������½���
            {
                g_text_end_flag = TRUE;
            }
            else if (end_mode == TEXT_END_READ_ERR)//��ȡ����ʧ�ܣ��˳�
            {
                result = RESULT_ERROR;
                break;//�˳��ؼ�
            }
            else
            {
                g_file_offset += page_bytes;
                if (g_file_offset >= text_file.file_length)
                {
                    g_text_end_flag = TRUE;
                }
                else
                {
                    g_text_end_flag = FALSE;
                }
            }

            text_decode.valid = TRUE;
            g_need_draw = FALSE;
        }

        result = textread_msg_deal();
        if (result != RESULT_NULL)
        {
            break;
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    //�ر��ı��ļ�
    sys_sd_fclose(sd_file_handle);
    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}

/*! \endcond */
