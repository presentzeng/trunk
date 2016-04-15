/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-18 21:56     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_directory.c
 * \brief    gui ���������ļ�������ؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ���ļ�������ؼ��������ļ�ϵͳ�������б��ղؼе��ļ������
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��ui������applib��common_func��enhanced_bs���ȵȡ�
 * \version  1.0
 * \date     2011-9-18
 *******************************************************************************/

#include "common_ui.h"

static void __directory_scroll_handle(void);
void load_dir_title_data(dir_control_t *p_dir_control);
void load_dir_item_data(dir_control_t *p_dir_control);
bool match_list_menu(uint8 layer, menu_title_data_t* title, list_menu_t *menu, uint8 count);
void get_dir_menu_item(file_brow_t *file_list_item, menu_title_data_t *p_dir_menu_title, uint16 index);
void update_decode_param(brow_decode_t *decode_param, dir_control_t *p_dir_control, uint16 lines);
app_result_e directory_msg_deal(file_path_info_t *path_info, dir_control_t *p_dir_control);

/*! \cond COMMON_API */

/*!
 *  \brief
 *  g_list_buffer���ļ��������ǰ�б����������
 */
uint8 g_list_buffer[LIST_ITEM_BUFFER_LEN * (LIST_NUM_ONE_PAGE_MAX + 1)] _BANK_DATA_ATTR_;
listbox_private_t listbox_data _BANK_DATA_ATTR_;

extern void update_listbox_private(dir_control_t *p_dir_control, listbox_private_t *dir_private)__FAR__;
extern void read_history_item(uint8 layer_no, dir_control_t *p_dir_control)__FAR__;
extern void write_history_item(uint8 layer_no, dir_control_t *p_dir_control, bool update)__FAR__;
extern app_result_e init_directory_env(dir_control_t *p_dir_control, uint8 *list_buffer,
        file_path_info_t *path_info)__FAR__;
extern bool init_dir_control(dir_control_t *p_dir_control)__FAR__;
extern app_result_e directory_msg_deal_gui(file_path_info_t *path_info, dir_control_t *p_dir_control,
        msg_apps_type_e gui_event) __FAR__;

/******************************************************************************/
/*!
 * \par  Description:
 *    �ļ���Ŀ¼���б�ؼ�������ʵ�֡�
 * \param[in]    style_id ui editor������Ƶ��ļ���Ŀ¼������ؼ�ģ��
 * \param[in]    path_info Ҫ�����ļ���Ŀ¼�������·���ṹ��
 * \param[in]    dir_com ָ���ļ���Ŀ¼���б�ؼ���ʼ���ṹ����������ϲ���ļ���Ŀ¼���б�Ŀ�����������
 * \param[out]   path_info ѡ���ļ���Ŀ¼�󣬷����ļ���Ŀ¼�������·���ṹ��
 * \return       app_result_e
 * \retval           RESULT_XXX_PLAY ѡ��ĳ��������ļ����в��ŷ���
 * \retval           RESULT_CONFIRM ȷ��ѡ���ļ��еȷ���
 * \retval           RESULT_REDRAW ���𼶣�ȡ������AP���ػ�UI
 * \retval           RESULT_DIR_ERROR_ENTER_DIR ���󷵻أ�����Ŀ¼���󣬰��� root
 * \retval           RESULT_DIR_ERROR_NO_FILE ���󷵻أ���Ŀ¼��û���ļ����ļ���
 * \retval           RESULT_DIR_ERROR_SETLOC ���󷵻أ����㲻�Ǹ�Ŀ¼����£�set location ʧ��
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \ingroup      controls
 * \par          exmaple code
 * \code
 dir_com_data_t dir_com;
 app_result_e result;

 dir_com.browse_mode = 0;
 dir_com.menulist_cnt = 0;
 dir_com.root_layer = 0;
 dir_com.del_back = FALSE;
 dir_com.list_option_func = commondir_option_func;
 dir_com.menulist = NULL;
 result = gui_directory(DIRLIST, &path_info, &dir_com);
 if(result == RESULT_MUSIC_PLAY)//ѡ�����ֽ��в���
 {

 }
 else if(result == RESULT_REDRAW)//ȡ�����أ��ػ�UI
 {

 }
 else if(result == RESULT_DIR_ERROR_NO_FILE)//��Ŀ¼û���ļ����ļ���
 {

 }
 else if(result == RESULT_DIR_ERROR_ENTER_DIR)//Ŀ¼�л��������󣬽��д�����
 {

 }
 else if(result == RESULT_DIR_ERROR_SETLOC)//���㲻�Ǹ�Ŀ¼����£�set location ʧ��
 {

 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 * \li  ����֧�����ļ�����б�����Ӳ˵��Ŀǰֻ����֧����� 1 ��������һҳ��Ĳ˵���
 *      ���û����Ӳ˵���Ļ�������� menulist ��ΪNULL������ menulist_cnt ��Ϊ0��
 *******************************************************************************/
app_result_e gui_directory(uint16 style_id, file_path_info_t *path_info, dir_com_data_t *dir_com)
{
    style_infor_t style_infor;
    app_result_e result = RESULT_NULL;

    int8 scroll_timer = -1;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;

    dir_history_valid = FALSE;
    style_infor.style_id = style_id;
    style_infor.type = UI_COM;

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_DIR);

    gui_get_headbar_mode(&mode_backup, &icon_backup);

    //countΪ0��ʾ��ǰ�ļ��б�û��Ƕ��˵�
    dir_menu_title.count = 0;

    //��ȡ one_page_count
    ui_get_listbox_attrb(&style_infor, &dir_control.one_page_count, 0);
    dir_control.dir_disk = path_info->file_path.dirlocation.disk;
    dir_control.file_source = path_info->file_source;
    dir_control.dir_com = dir_com;

    //��ʼ�� gui_directory �����������ʼ��ʧ�ܣ�path_info �ᱻ��ʼ��ָ�� ROOT
    result = init_directory_env(&dir_control, g_list_buffer, path_info);
    if (result != RESULT_NULL)
    {
        write_history_item(0, &dir_control, TRUE);
        change_app_timer_tag(tag_backup);
        return result;
    }

    //��Ŀ¼��û���ļ����ļ������
    if ((g_dir_browser.layer_no == 0) && ((g_dir_browser.dir_total + g_dir_browser.file_total) == 0))
    {
        //��������Ŀ¼û���ļ�����ֲ˵����������в˵������˳��������˳�
        if (!(((dir_com->browse_mode & 0x80) != 0) && (dir_control.dir_com->menulist_cnt != 0)))
        {
            //���� RESULT_DIR_ERROR_NO_FILE
            write_history_item(0, &dir_control, TRUE);
            change_app_timer_tag(tag_backup);
            return RESULT_DIR_ERROR_NO_FILE;
        }
    }

    speed_control = 0;
    dir_control.enter_dir = TRUE;
    dir_control.draw_mode = LIST_DRAW_ALL;
    //�����ļ����ѭ��
    while (1)
    {
        //��ȡ�б���
        if (dir_control.update_mode != LIST_UPDATE_NULL)
        {
            if (dir_control.enter_dir == TRUE)
            {
                //����Ŀ¼ͷ������ǰ�б��в˵�����ڴ˼��ز˵���
                load_dir_title_data(&dir_control);

                dir_control.enter_dir = FALSE;
            }

            //�����ļ���¼������
            load_dir_item_data(&dir_control);

            dir_control.update_mode = LIST_UPDATE_NULL;
        }

        if ((dir_control.draw_mode != LIST_DRAW_NULL) && (get_screensave_state() == FALSE))
        {
            if (dir_control.draw_mode == LIST_DRAW_ALL)
            {
                ui_set_backgd_color(Color_BACKCOLOR_def);
                ui_clear_screen(NULL);
                gui_set_headbar_mode(HEADBAR_MODE_NORMAL, this_headbar_icon_id);
            }

            update_listbox_private(&dir_control, &listbox_data);
            if (ui_show_listbox(&style_infor, &listbox_data, (uint32) dir_control.draw_mode) == UI_NEED_SCROLL)
            {
                if (scroll_timer == -1)//δ����������
                {
                    scroll_timer = set_app_timer(APP_TIMER_ATTRB_UI, 80, __directory_scroll_handle);
                }
            }
            else
            {
                //ɾ����ʱ������ʹû����Ҳû����
                kill_app_timer(scroll_timer);
                scroll_timer = -1;
            }

            dir_control.draw_mode = LIST_DRAW_NULL;
        }

        result = directory_msg_deal(path_info, &dir_control);
        if (result != RESULT_NULL)
        {
            break;
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    write_history_item(0, &dir_control, TRUE);
    kill_app_timer(scroll_timer);
    change_app_timer_tag(tag_backup);
    return result;
}

/*! \endcond */

/*! \cond */

static void __directory_scroll_handle(void)
{
    ui_show_listbox(NULL, NULL, LIST_DRAW_ACTIVE_SCROLL);
}

//directory gui app msg deal
app_result_e directory_msg_deal(file_path_info_t *path_info, dir_control_t *p_dir_control)
{
    input_gui_msg_t input_msg; //��������Ϣ����gui��Ϣ
    msg_apps_type_e gui_event; //gui ��Ϣӳ���¼�
    private_msg_t private_msg; //˽����Ϣ
    app_result_e result = RESULT_NULL;

    //��ȡgui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)//��gui��Ϣ
    {
        //���а���ӳ��
        if (com_key_mapping(&input_msg, &gui_event, directory_key_map_list) == TRUE)
        {
            result = directory_msg_deal_gui(path_info, p_dir_control, gui_event);
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
                //�����ػ浱ǰ�б�
                fsel_browser_enter_dir(CUR_DIR, 0, &g_dir_browser);
                p_dir_control->update_mode = LIST_UPDATE_REUPDATE;
                p_dir_control->draw_mode = LIST_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
            }
            else if (result == RESULT_NULL)
            {
                //�л���Ӧ��control bank�������bank data
                p_dir_control->update_mode = LIST_UPDATE_REUPDATE;
                result = RESULT_NULL;//���˳��ؼ�
            }
            else
            {
                //do nothing for QAC
            }
        }
    }

    return result;
}

//����Ŀ¼ͷ������ǰ�б��в˵�����ڴ˼��ز˵���
void load_dir_title_data(dir_control_t *p_dir_control)
{
    dir_com_data_t *t_dir_com = p_dir_control->dir_com;

    //���ݵ�ǰĿ¼���ƽṹ�壬�����Ϳ�������һ��Ŀ¼�����½����Ŀ¼
    libc_memcpy(&last_dir_control, p_dir_control, sizeof(dir_control_t));

    //ƥ���Ƿ�Ƕ��˵��б�
    dir_menu_title.count = 0;
    if (t_dir_com->menulist != NULL)
    {
        t_dir_com->menulist_cnt = 1;//����ֻ֧��1�������ҽ�֧�ֲ�����һҳ�˵���
        match_list_menu(g_dir_browser.layer_no, &dir_menu_title, t_dir_com->menulist, t_dir_com->menulist_cnt);
    }

    //�����б������������� dir_control ���г�Ա
    dir_control.list_total = g_dir_browser.dir_total + g_dir_browser.file_total + dir_menu_title.count;

    //��ȡ��ǰ����ļ����·����¼
    read_history_item(g_dir_browser.layer_no, p_dir_control);

    //�������� dir_control ��Ա
    init_dir_control(p_dir_control);
}

//�����ļ���¼������
void load_dir_item_data(dir_control_t *p_dir_control)
{
    file_brow_t *cur_file_list;
    brow_decode_t decode_param;

    //���� decode_param������ǰ����Ч�����б���
    update_decode_param(&decode_param, p_dir_control, line_cnt);

    //��ȡ���ļ���¼��
    //1.�����Ƿ�ǰ�㼶��Ҫ����б�˵����˵���������¶�����ظ���
    if ((p_dir_control->dir_com->menulist != NULL) && (decode_param.top <= dir_menu_title.count))
    {
        //�ȴӲ˵��ж�ȡ������
        while ((decode_param.num > 0) && (decode_param.top <= dir_menu_title.count) && (decode_param.top
                <= p_dir_control->bottom))
        {
            cur_file_list = g_file_list + (decode_param.top - p_dir_control->top);
            get_dir_menu_item(cur_file_list, &dir_menu_title, decode_param.top - 1);
            decode_param.top++;
            decode_param.num--;
        }
    }

    //2.�����Ƿ���Ҫ������ļ���¼��
    if (decode_param.num > 0)
    {
        cur_file_list = g_file_list + (decode_param.top - p_dir_control->top);
        //��ȡ�����ļ���¼�����ǰ����Ч�����б���
        decode_param.top = decode_param.top - dir_menu_title.count;//��ȥ�˵���
        fsel_browser_get_items(&decode_param, cur_file_list);
    }
}

//���� brow_decode_t �ṹ�壬list_cnt���� LIST_UPDATE_NEXT �� LIST_UPDATE_PREV ��Ч
void update_decode_param(brow_decode_t *decode_param, dir_control_t *p_dir_control, uint16 lines)
{
    decode_param->option = (uint8) p_dir_control->update_mode;
    switch (p_dir_control->update_mode)
    {
        case LIST_UPDATE_NEXT:
        decode_param->top = p_dir_control->bottom - (lines - 1);
        decode_param->num = (uint8) lines;
        break;
        case LIST_UPDATE_PREV:
        decode_param->top = p_dir_control->top;
        decode_param->num = (uint8) lines;
        break;
        default://ȫ������
        decode_param->top = p_dir_control->top;
        decode_param->num = (uint8) (p_dir_control->bottom - p_dir_control->top + 1);
        break;
    }
}

//ƥ���ļ�������˵��б����飬��֧�� 1 ���˵��б�
bool match_list_menu(uint8 layer, menu_title_data_t* title, list_menu_t *menu, uint8 count)
{
    //����ƥ��㼶������ƥ��ɹ�
    if ((menu->layer == V_U8_INVALID) || (menu->layer == layer))
    {
        //���ز˵�ͷ����
        com_get_confmenu_title((uint8) menu->list_menu, title);
        return TRUE;
    }

    return FALSE;
}

//��ȡ�˵����ַ�������
void get_dir_menu_item(file_brow_t *file_list_item, menu_title_data_t *p_dir_menu_title, uint16 index)
{
    menu_item_data_t dir_menu_item;
    string_desc_t str_desc;

    file_list_item->name_buf[0] = 0xff;
    file_list_item->name_buf[1] = 0xfe;
    str_desc.data.str = file_list_item->name_buf + 2;
    str_desc.length = LIST_ITEM_BUFFER_LEN - 2;

    //��ȡ�˵���
    com_get_confmenu_item(p_dir_menu_title, index, &dir_menu_item);
    //��ȡ�ַ���ID code
    ui_get_multi_string_unicode(dir_menu_item.str_id, &str_desc);
}

/*! \endcond */
