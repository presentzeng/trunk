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
 * \file     _ui_directory_sub2.c
 * \brief    commonUI �ļ�������ؼ�������ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ���ļ������GUI�������ļ�ϵͳ�������б��ղؼе��ļ������
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�listbox��ɿؼ���ʾ������
 * \version  1.0
 * \date     2011-9-18
 *******************************************************************************/

#include "common_ui.h"

void write_history_item(uint8 layer_no, dir_control_t *p_dir_control, bool update);

//��ȡ·����ʷ��¼
void read_history_item(uint8 layer_no, dir_control_t *p_dir_control)
{
    history_index_t temp_index;
    history_item_t temp_history_item;

    temp_index.type = PATH_HISTORY_DIR;
    temp_index.app_id = g_this_app_info->app_id;
    temp_index.path_id = p_dir_control->dir_disk;
    temp_index.layer = layer_no;
    com_get_history_item(&temp_index, &temp_history_item);

    if (p_dir_control->dir_com != NULL)
    {
        if (p_dir_control->dir_com->del_back == TRUE)
        {
            uint16 tmp_bottom;

            //ɾ�������·�����䣬�������û���ļ��油����ҪŲ���б������������
            tmp_bottom = temp_history_item.top + p_dir_control->one_page_count - 1;
            if (tmp_bottom >= (p_dir_control->list_total + 1))//list_totalΪɾ�����list_total���ʼ�1
            {
                //��δ�г���һ�ǰ�������ļ����油�������б������ƶ�һ��
                if (temp_history_item.top > 1)
                {
                    temp_history_item.top--;
                    temp_history_item.list_no--;
                }
                else//ǰ��û���ļ��油���Ѻ�����ǰŲ��
                {
                    //ɾ�����һ���Ҫ�ı� list_no
                    if (temp_history_item.list_no == tmp_bottom)
                    {
                        temp_history_item.list_no--;
                    }
                }
            }

            p_dir_control->dir_com->del_back = FALSE;
        }
    }

    if (temp_history_item.top == V_U16_INVALID)
    {
        p_dir_control->top = 1;
        p_dir_control->list_no = 1;
    }
    else if ((temp_history_item.top == 0) || (temp_history_item.list_no == 0) || (temp_history_item.top
            > p_dir_control->list_total) || (temp_history_item.list_no > p_dir_control->list_total)
            || (temp_history_item.top > temp_history_item.list_no) || ((temp_history_item.list_no
            - temp_history_item.top) > (p_dir_control->one_page_count - 1)))
    {
        //·�������������ǿ��ָ��ͷ��������д��·�����䣬��ǿ�ưѺ���㼶��Ϊ��Ч
        p_dir_control->top = 1;
        p_dir_control->list_no = 1;
        write_history_item(layer_no, p_dir_control, FALSE);
    }
    else
    {
        p_dir_control->top = temp_history_item.top;
        p_dir_control->list_no = temp_history_item.list_no;
    }
}

//����·����ʷ��¼
void write_history_item(uint8 layer_no, dir_control_t *p_dir_control, bool update)
{
    history_index_t temp_index;
    history_item_t temp_history_item;

    temp_history_item.top = p_dir_control->top;
    temp_history_item.list_no = p_dir_control->list_no;

    temp_index.type = PATH_HISTORY_DIR;
    temp_index.app_id = g_this_app_info->app_id;
    temp_index.path_id = p_dir_control->dir_disk;
    temp_index.layer = layer_no;
    if (update == FALSE)
    {
        com_set_history_item(&temp_index, &temp_history_item);
    }
    else
    {
        com_update_path_history(&temp_index);
    }
}

//��ʼ�� gui_directory ������������ʼ��������������Ŀ¼
app_result_e init_directory_env(dir_control_t *p_dir_control, uint8 *list_buffer, file_path_info_t *path_info)
{
    uint8 i;

    //��ʼ�б�������ռ�
    g_dir_browser.name_buf = list_buffer + 0;
    g_dir_browser.name_len = LIST_ITEM_BUFFER_LEN;
    for (i = 1; i <= LIST_NUM_ONE_PAGE_MAX; i++)
    {
        g_file_list[i - 1].name_buf = list_buffer + (LIST_ITEM_BUFFER_LEN * i);
        g_file_list[i - 1].name_len = LIST_ITEM_BUFFER_LEN;
    }

    //��·���ĵ�һ��Ŀ¼���
    if ((p_dir_control->dir_com->browse_mode & 0x7f) == 0)
    {
        if (p_dir_control->dir_com->root_layer == 0)
        {
            //���벢��ȡ��Ŀ¼��Ŀ¼�ṹ��
            if (fsel_browser_enter_dir(ROOT_DIR, 0, &g_dir_browser) == FALSE)
            {
                return RESULT_DIR_ERROR_ENTER_DIR;
            }
        }
        else
        {
            //���벢��ȡ��ǰĿ¼��Ŀ¼�ṹ�壬�û����� gui_directory ֮ǰ���뱣֤�Ѿ���λ��ָ��Ŀ¼
            if (fsel_browser_enter_dir(CUR_DIR, 0, &g_dir_browser) == FALSE)
            {
                return RESULT_DIR_ERROR_ENTER_DIR;
            }
        }
        dir_control.update_mode = LIST_UPDATE_HEAD;
    }
    else//ֱ������·��ָ��Ŀ¼���
    {
        if (fsel_browser_set_location(&(path_info->file_path), path_info->file_source) == TRUE)
        {
            //��ȡ��ǰ·����Ŀ¼�ṹ��
            if (fsel_browser_enter_dir(CUR_DIR, 0, &g_dir_browser) == FALSE)
            {
                return RESULT_DIR_ERROR_ENTER_DIR;
            }
            dir_control.update_mode = LIST_UPDATE_REUPDATE;
        }
        else
        {
            if (p_dir_control->dir_com->root_layer == 0)
            {
                //���벢��ȡ��Ŀ¼��Ŀ¼�ṹ��
                if (fsel_browser_enter_dir(ROOT_DIR, 0, &g_dir_browser) == FALSE)
                {
                    return RESULT_DIR_ERROR_ENTER_DIR;
                }
                dir_control.update_mode = LIST_UPDATE_HEAD;

                //��ȡ ROOT �ļ�·����ȷ���´ν���ʱ���óɹ�
                fsel_browser_get_location(&(path_info->file_path), path_info->file_source);

                //�Ҳ���·���������Ӧ���ļ��������
                com_clear_app_history(PATH_HISTORY_DIR, g_this_app_info->app_id, p_dir_control->dir_disk);
            }
            else
            {
                //���㲻�Ǹ�Ŀ¼�����󷵻أ����ϲ��Լ�����
                return RESULT_DIR_ERROR_SETLOC;
            }
        }
    }

    return RESULT_NULL;
}

//ͬ������listbox˽�����ݽṹ
void update_listbox_private(dir_control_t *p_dir_control, listbox_private_t *dir_private)
{
    uint8 index;

    dir_private->title.data.str = g_dir_browser.name_buf;
    dir_private->title.length = LIST_ITEM_BUFFER_LEN;
    if ((dir_private->title.data.str[0] == 0xff) && (dir_private->title.data.str[1] == 0xfe))
    {
        dir_private->title.language = UNICODEDATA;
    }
    else
    {
        //����������ӽ�����
        dir_private->title.data.str[11] = '\0';
        dir_private->title.language = (int8) sys_comval->language_id;
    }

    dir_private->item_valid = p_dir_control->bottom - p_dir_control->top + 1;
    dir_private->active = p_dir_control->active - 1;
    dir_private->old = p_dir_control->old - 1;
    dir_private->list_no = p_dir_control->list_no - 1;
    dir_private->list_total = p_dir_control->list_total;

    for (index = 0; index < dir_private->item_valid; index++)
    {
        dir_private->items[index].data.str = g_file_list[index].name_buf;
        dir_private->items[index].length = LIST_ITEM_BUFFER_LEN;
        if ((dir_private->items[index].data.str[0] == 0xff) && (dir_private->items[index].data.str[1] == 0xfe))
        {
            dir_private->items[index].language = UNICODEDATA;
        }
        else if ((dir_private->items[index].data.str[0] == 0xef) && (dir_private->items[index].data.str[1] == 0xbb)
                && (dir_private->items[index].data.str[2] == 0xbf))
        {
            dir_private->items[index].language = UTF_8DATA;
        }
        else
        {
            dir_private->items[index].language = (int8) sys_comval->language_id;
        }

        if ((p_dir_control->top + index) <= dir_menu_title.count)
        {
            dir_private->items[index].argv = ITEMS_FRONT_TAG_ISNOT_DISP;
        }
        else if ((p_dir_control->top + index) <= (g_dir_browser.dir_total + dir_menu_title.count))
        {
            dir_private->items[index].argv = ITEMS_FRONT_TAG_IS_DISP | ITEMS_FRONT_TAG_INDEX;
        }
        else
        {
            dir_private->items[index].argv = ITEMS_FRONT_TAG_ISNOT_DISP;
        }
    }
}
