/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-directory
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-18 21:56     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_directory2.c
 * \brief    commonUI �ļ�������ؼ� gui ��Ϣ�ַ�����ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ���ļ������GUI�������ļ�ϵͳ�������б��ղؼе��ļ������
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�listbox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2012-2-21
 *******************************************************************************/

#include "common_ui.h"

/*!
 *  \brief
 *  ext_vec_rom��ROM�еĺ�׺�����飬������ 31 ����׺��
 *
 */
const char ext_vec_rom[32][4] =
{ "***", "MP3", "WMA", "WAV", "AAC", "FLA", "APE", "AVI", "TXT", "JPG", "JPE", "BMP", "GIF", "LRC", "ASF", "OGG",
        "WMV", "AMV", "AAX", "AA ", "MP1", "MP2", "M4A", "POS", "ACT", "** ", "AIF", "** ", "** ", "**", "*f", "*d" };

/*!
 *  \brief
 *  bitmap_vec��ROM�еĺ�׺�������bitmap���ϣ�bitmap��ROM�еĺ�׺����Ӧ��ϵ�ǣ�
 *  ��bit30��ʼ�����λ��Ӧ��һ����׺������������
 */
const uint32 bitmap_vec[8] =
{
/*! ���ֺ�׺��bitmap */
MUSIC_BITMAP,
/*! �����鼮��׺��bitmap */
AUDIBLE_BITMAP,
/*! ��Ƶ�ļ���׺��bitmap */
VIDEO_BITMAP,
/*! ͼƬ�ļ���׺��bitmap */
PICTURE_BITMAP,
/*! �ı��ļ���׺��bitmap */
TEXT_BITMAP,
/*! �����ļ���Ŀ¼bitmap */
ALLALL_BITMAP,
/*! �����ļ�bitmap */
ALLFILE_BITMAP,
/*! ����Ŀ¼bitmap */
ALLDIR_BITMAP, };

/*!
 *  \brief
 *  result_vec����Ӧ�Ľ����ʶ���ϣ������ֶ�ӦRESULT_MUSIC_PLAY��
 */
const app_result_e result_vec[6] =
{
/*! �����ļ�������Ϣ */
RESULT_MUSIC_PLAY,
/*! �����鼮�ļ�������Ϣ */
RESULT_AUDIBLE_PLAY,
/*! ��Ƶ�ļ�������Ϣ */
RESULT_MOVIE_PLAY,
/*! ͼƬ�ļ�������Ϣ */
RESULT_PICTURE_PLAY,
/*! �ı��ļ�������Ϣ */
RESULT_EBOOK_PLAY,
/*! ����Ϣ */
RESULT_NULL, };

uint8 dir_file_type(char *ext);
void get_dir_menu_item(file_brow_t *file_list_item, menu_title_data_t *p_dir_menu_title, uint16 index);
menu_cb_func get_dir_menu_item_func(menu_title_data_t *p_dir_menu_title, uint16 index);
void update_dir_control_after_del(dir_control_t *p_dir_control);
app_result_e directory_option_deal(file_path_info_t*path_info, dir_control_t*p_dir_control);

extern void dir_select_next(file_brow_t *file_list, dir_control_t *p_dir_control)__FAR__;
extern void dir_select_prev(file_brow_t *file_list, dir_control_t *p_dir_control)__FAR__;
extern void write_history_item(uint8 layer_no, dir_control_t *p_dir_control, bool update)__FAR__;
extern void read_history_item(uint8 layer_no, dir_control_t *p_dir_control)__FAR__;

//��ȡ��׺����Ӧ��Ӧ����ţ���result_vec���������ö�Ӧ��app_result_e������Ϣ
//����0-4Ϊ֧�ֵ��ļ����ͣ�5��ʾ��֧�ֵ��ļ����ͣ���ӦRESULT_NULL��Ϣ
uint8 dir_file_type(char *ext)
{
    uint8 i, j;
    uint32 bitmap;

    //��Ӧ��ƥ��
    //for (i = 0; i < 5; i++)
    for (i = 0; i < 2; i++) //us216a��֧��ͼƬ����Ƶ��EBOOK�����ļ�
    {
        bitmap = 0x80000000;
        for (j = 0; j < 32; j++)
        {
            if ((bitmap_vec[i] & bitmap) != 0x0)
            {
                if (libc_strncmp(ext, ext_vec_rom[j], 4) == 0)
                {
                    return i;
                }
            }
            bitmap = bitmap >> 1;
        }
    }

    return 5;
}

//ɾ���ļ����ļ��л��б������ dir_control_t �ṹ��
void update_dir_control_after_del(dir_control_t *p_dir_control)
{
    //��δ�г����һ����滹���ļ��油��ֻ��Ҫ�� list_total ��ȥ1 ����
    if (p_dir_control->bottom < p_dir_control->list_total)
    {
        p_dir_control->list_total--;
    }
    else//����û���ļ��油����ҪŲ���б�
    {
        //��δ�г���һ�ǰ�������ļ����油�������б������ƶ�һ��
        if (p_dir_control->top > 1)
        {
            p_dir_control->list_total--;
            p_dir_control->top--;
            p_dir_control->bottom--;
            p_dir_control->list_no--;
        }
        else//ǰ��û���ļ��油���Ѻ�����ǰŲ��
        {
            //ɾ�����һ���Ҫ�ı� list_no
            if (p_dir_control->list_no == p_dir_control->bottom)
            {
                p_dir_control->list_no--;
                p_dir_control->active--;
            }
            p_dir_control->list_total--;
            p_dir_control->bottom--;
        }
    }
    p_dir_control->old = p_dir_control->active;
}

//directory gui msg--option deal
app_result_e directory_option_deal(file_path_info_t *path_info, dir_control_t *p_dir_control)
{
    app_result_e result = RESULT_NULL;
    uint16 tmp_list_no = p_dir_control->list_no - dir_menu_title.count;

    //ע�⣺�ú��������޸ĵ� path_info����ʹ fsel_browser_enter_dir(CUR_DIR, 0, &g_dir_browser);
    //��������ı䣬�û�Ӧ���������������Ѹ�����ʵ�� list_option_func ����
    result = p_dir_control->dir_com->list_option_func(path_info, tmp_list_no);
    //ɾ����ǰ�ļ��л��б����ļ������ļ��л��б������б�
    if ((result == RESULT_DELETE_FILE) || (result == RESULT_DELETE_DIRLIST) || (result == RESULT_DELETE_DIRLIST_NOSELF))
    {
        if (result == RESULT_DELETE_DIRLIST_NOSELF)
        {
            result = RESULT_REDRAW;
        }
        else
        {
            update_dir_control_after_del(p_dir_control);
            if (p_dir_control->list_total == 0)//��ǰ�б������б�������˵������в˵�����Ȼ���Խ�����ʾ��
            {
                //��ʾ�Ҳ����ļ�
                //gui_dialog_msg (DIALOG_MSG, DIALOG_INFOR_WAIT, S_FILE_NO_FILE);
                //������һ��
                //1.����Ѿ�������㣬��ʾ�����Ѿ�û�з����������ļ������ء����ļ���
                if (g_dir_browser.layer_no == p_dir_control->dir_com->root_layer)
                {
                    //�Ѿ������Ŀ¼�����ڵ㣬����
                    result = RESULT_DIR_ERROR_NO_FILE;
                }
                //2.������һ��Ŀ¼
                else
                {
                    if (fsel_browser_enter_dir(PARENT_DIR, 0, &g_dir_browser) == TRUE)
                    {
                        p_dir_control->enter_dir = TRUE;
                        p_dir_control->update_mode = LIST_UPDATE_HEAD;
                        p_dir_control->draw_mode = LIST_DRAW_ALL;
                        result = RESULT_NULL;//���˳��ؼ�
                    }
                    else
                    {
                        result = RESULT_DIR_ERROR_ENTER_DIR;
                    }
                }
            }
            else//�����ļ����ػ浱ǰ�б�
            {
                result = RESULT_REDRAW;
            }
        }

    }

    if (result == RESULT_REDRAW)
    {
        //�����ػ浱ǰ�б�
        fsel_browser_enter_dir(CUR_DIR, 0, &g_dir_browser);
        p_dir_control->update_mode = LIST_UPDATE_REUPDATE;
        p_dir_control->draw_mode = LIST_DRAW_ALL;
        result = RESULT_NULL;//���˳��ؼ�
    }

    return result;
}

//directory gui app msg deal--gui
app_result_e directory_msg_deal_gui(file_path_info_t *path_info, dir_control_t *p_dir_control,
        msg_apps_type_e gui_event)
{
    file_brow_t *cur_file;//��ǰѡ����ļ��ṹ��
    app_result_e result = RESULT_NULL;
    bool redraw_all = FALSE;

    //GUI������Ϣ����
    switch (gui_event)
    {
        case EVENT_DIRECTORY_SELECT_NEXT:
        //�����и����б�����ƽṹ�����ƶ���Ч�ļ���¼��
        dir_select_next(g_file_list, p_dir_control);
        break;

        case EVENT_DIRECTORY_SELECT_PREV:
        //�����и����б�����ƽṹ�����ƶ���Ч�ļ���¼��
        dir_select_prev(g_file_list, p_dir_control);
        break;

        case EVENT_DIRECTORY_SELECT_STOP:
        //����ٶȿ��Ʊ�������ͷ��ʼ����
        speed_control = 0;
        break;

        case EVENT_DIRECTORY_BACK_PARENT:
        if (fsel_module_type != FSEL_MODULE_FAVOR)
        {
            //���浱ǰ����ļ������¼
            write_history_item(g_dir_browser.layer_no, p_dir_control, FALSE);
        }

        if (g_dir_browser.layer_no == p_dir_control->dir_com->root_layer)
        {
            //�Ѿ������Ŀ¼�����ڵ㣬����
            result = RESULT_REDRAW;
        }
        else
        {
            //������һ��Ŀ¼
            if (fsel_browser_enter_dir(PARENT_DIR, 0, &g_dir_browser) == TRUE)
            {
                p_dir_control->enter_dir = TRUE;
                p_dir_control->update_mode = LIST_UPDATE_HEAD;
                p_dir_control->draw_mode = LIST_DRAW_ALL;
            }
            else
            {
                result = RESULT_DIR_ERROR_ENTER_DIR;
            }
        }
        break;

        case EVENT_DIRECTORY_ENTER_SON:
        //���浱ǰ����ļ������¼
        write_history_item(g_dir_browser.layer_no, p_dir_control, FALSE);
        if ((p_dir_control->active < 1) || (p_dir_control->active > LIST_NUM_ONE_PAGE_MAX))//for QAM
        {
            result = RESULT_DIR_ERROR_ENTER_DIR;
            break;
        }
        cur_file = &(g_file_list[p_dir_control->active - 1]);

        //ѡ��˵���
        if (p_dir_control->list_no <= dir_menu_title.count)
        {
            menu_cb_func func;
            //�Ӳ˵��ж�ȡ��ǰ��p_dir_control->list_no��Ȼ��ִ�лص�����
            func = get_dir_menu_item_func(&dir_menu_title, p_dir_control->list_no - 1);
            if (func != NULL)
            {
                result = func(NULL);
                if (result == RESULT_REDRAW)
                {
                    //�����ػ浱ǰ�б�
                    redraw_all = TRUE;
                }
            }
        }
        //ѡ���ļ���
        else if (p_dir_control->list_no <= (g_dir_browser.dir_total + dir_menu_title.count))
        {
            uint16 tmp_active;

            if (g_dir_browser.layer_no >= (MAX_DIR_LAYER - 1))
            {
                //����8����Ŀ¼�����ش��󣬲������κη�Ӧ
                break;
            }

            //���������Ŀ¼�ϣ�������Ŀ¼
            tmp_active = p_dir_control->list_no - dir_menu_title.count;
            if (fsel_browser_enter_dir(SON_DIR, tmp_active, &g_dir_browser) == TRUE)
            {
                //�����ļ��У������룬��ʾ�Ҳ����ļ����ػ游Ŀ¼
                if ((g_dir_browser.dir_total == 0) && (g_dir_browser.file_total == 0))
                {
                    fsel_browser_enter_dir(PARENT_DIR, 0, &g_dir_browser);

                    //��ʾ�Ҳ����ļ�
                    gui_dialog_msg(DIALOG_MSG, DIALOG_INFOR_WAIT, S_FILE_NO_FILE);
                    //�����ػ浱ǰ�б�
                    redraw_all = TRUE;
                }
                else//�������ļ���
                {
                    p_dir_control->enter_dir = TRUE;
                    p_dir_control->update_mode = LIST_UPDATE_HEAD;
                    p_dir_control->draw_mode = LIST_DRAW_ALL;
                    result = RESULT_NULL;//���˳��ؼ�
                }
            }
            else
            {
                result = RESULT_DIR_ERROR_ENTER_DIR;
            }
        }
        //ѡ���ļ�
        else
        {
            //ѡ���ļ�����
            result = result_vec[dir_file_type(cur_file->ext)];
            if (result != RESULT_NULL)//ֻ����֧���ļ����Ͳ���
            {
                //����ѡ���ļ�
                fsel_browser_select(p_dir_control->list_no - dir_menu_title.count);
                //��ȡ��ǰ�ļ�·��
                fsel_browser_get_location(&(path_info->file_path), path_info->file_source);
            }
            else
            {
                //��ʾ�ļ���ʽ��֧��
                gui_dialog_msg(DIALOG_MSG, DIALOG_INFOR_WAIT, S_FILE_NO_SUPPORT);
                //�����ػ浱ǰ�б�
                redraw_all = TRUE;
            }
        }
        break;

        case EVENT_DIRECTORY_ENTER_OPTION:
        // ���뵯��ʽ option menu
        if (p_dir_control->dir_com->list_option_func != NULL)
        {
            result = directory_option_deal(path_info, p_dir_control);
            if (result != RESULT_NULL)
            {
                //���浱ǰ����ļ������¼
                write_history_item(g_dir_browser.layer_no, p_dir_control, FALSE);
            }
        }
        break;

        default:
        result = com_message_box(gui_event);
        if (result == RESULT_REDRAW)
        {
            //�����ػ浱ǰ�б�
            redraw_all = TRUE;
        }
        break;
    }

    if (redraw_all == TRUE)
    {
        fsel_browser_enter_dir(CUR_DIR, 0, &g_dir_browser);
        p_dir_control->update_mode = LIST_UPDATE_REUPDATE;
        p_dir_control->draw_mode = LIST_DRAW_ALL;
        result = RESULT_NULL;//���˳��ؼ�
    }

    return result;
}

//��ȡ�˵���ȷ��ִ�к��� menu_func
menu_cb_func get_dir_menu_item_func(menu_title_data_t *p_dir_menu_title, uint16 index)
{
    menu_item_data_t dir_menu_item;
    menu_cb_func func;

    //��ȡ�˵���
    com_get_confmenu_item(p_dir_menu_title, index, &dir_menu_item);
#ifndef PC
    func = dir_menu_item.menu_func;
#else
    func = this_get_menu_func(dir_menu_item.str_id, dir_menu_item.str_id_sub);
#endif

    return func;
}
