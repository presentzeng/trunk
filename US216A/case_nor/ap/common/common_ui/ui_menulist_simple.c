/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2012-6-28 9:13     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_menulist_simple.c
 * \brief    gui ���������򻯰�˵��б�ؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�ֿ����û��ģ�����·�����书�ܣ���֧�ֶ�̬�˵����ܵļ򻯰�
 *           �˵��б�ؼ������ֲ˵��б����������Ͳ˵��б���ֻ����һ��˵���ѡ����
 *           Ҷ�Ӳ˵����ִ���������˳���
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��ui������applib��common_func���ȵȡ�
 * \version  1.0
 * \date     2012-6-28
 *******************************************************************************/

#include "common_ui.h"

/*! \cond COMMON_API */

static listbox_private_t menu_private _BANK_DATA_ATTR_;
static void __menulist_scroll_handle(void);
static void __init_action_title(menu_title_action_t *title_action);
static void __load_conf_title_simple(menulist_control_t *p_ml_control, uint8 menu_id);
static void __load_conf_item_simple(menulist_control_t *p_ml_control);

#ifdef PC
extern __get_menu_func this_get_menu_func;
extern __get_callback_func this_get_callback_func;
extern __get_menu_option this_get_menu_option;
extern void load_menulist_simfunc(__get_menu_func func1, __get_callback_func func2, __get_menu_option func3);
#endif

extern void menulist_select_next(menulist_control_t *p_ml_control);
extern void menulist_select_prev(menulist_control_t *p_ml_control);
extern void update_menulist_private(menulist_control_t *p_ml_control, listbox_private_t *menu_private,
        menu_title_action_t *title_action);

//menulist gui app msg deal
app_result_e menulist_simple_msg_deal(menulist_control_t *p_ml_control, menu_title_action_t *title_action)
{
    input_gui_msg_t input_msg; //��������Ϣ����gui��Ϣ
    msg_apps_type_e gui_event; //gui ��Ϣӳ���¼�
    private_msg_t private_msg; //˽����Ϣ
    app_result_e result = RESULT_NULL;
    menu_item_data_t *active_menu;

#ifdef PC
    menu_cb_func menu_func;
#endif

    //��ȡgui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)//��gui��Ϣ
    {
        //���а���ӳ��
        if (com_key_mapping(&input_msg, &gui_event, menulist_key_map_list) == TRUE)
        {
            //GUI������Ϣ����
            switch (gui_event)
            {
                case EVENT_MENULIST_SELECT_NEXT:
                //�л�����һ���˵���
                menulist_select_next(p_ml_control);
                break;

                case EVENT_MENULIST_SELECT_PREV:
                //�л�����һ���˵���
                menulist_select_prev(p_ml_control);
                break;

                case EVENT_MENULIST_ENTER_SON:
                active_menu = &g_menu_items[p_ml_control->active];
                //����Ҷ�Ӳ˵�����
                if ((active_menu->child_index & MENU_TYPE_LEAF) == MENU_TYPE_LEAF)
                {
                    __init_action_title(title_action);
#ifndef PC
                    result = active_menu->menu_func(title_action);
#else
                    menu_func = this_get_menu_func(active_menu->str_id, active_menu->str_id_sub);
                    result = menu_func(title_action);
#endif

                    if ((result == RESULT_MENU_EXIT) || (result == RESULT_MENU_PARENT) || (result == RESULT_MENU_CUR)
                            || (result == RESULT_MENU_REDRAW_PARENT) || (result == RESULT_REDRAW) || (result
                            == RESULT_NULL))
                    {
                        result = RESULT_REDRAW;
                    }
                }
                else//ֻ֧��Ҷ�Ӳ˵�����˵�������������˳�
                {
                    //�˳��˵�������
                    result = RESULT_REDRAW;
                }
                break;

                case EVENT_MENULIST_BACK_PARENT:
                case EVENT_MENULIST_ENTER_OPTION://option�¼�����parent�¼�����
                //�˳��˵�������
                result = RESULT_REDRAW;
                break;

                default:
                result = com_message_box(gui_event);
                if (result == RESULT_REDRAW)
                {
                    p_ml_control->draw_mode = LIST_DRAW_ALL;
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
                //������ʾ��ʾ�������ػ�UI
                p_ml_control->draw_mode = LIST_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
            }
        }
    }

    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �˵��б�ؼ�������ʵ�֡�
 * \param[in]    style_id ui editor������ƵĲ˵��б�ؼ�ģ��
 * \param[in]    menu_com ָ��˵��б�ؼ���ʼ���ṹ����������ϲ�Բ˵��б�Ŀ�����������
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_MENU_EXIT �������ڲ�ʹ�õ���Ϣ���˳��˵����أ���������һ���˵���������ֱ���˳���������
 * \retval           RESULT_MENU_PARENT �������ڲ�ʹ�õ���Ϣ���˻���һ����Ҷ�Ӳ˵����ȷ����������
 * \retval           RESULT_MENU_CUR �������ڲ�ʹ�õ���Ϣ�����½��뵱ǰ���˵�����ִ��ȷ��������Ҫ���붯̬�˵�ʱ����
 * \retval           RESULT_MENU_SON �������ڲ�ʹ�õ���Ϣ��������һ�������˵����ȷ���������أ��򷵻�RESULT_NULL
 * \retval           RESULT_MENU_REDRAW_PARENT �������ڲ�ʹ�õ���Ϣ���ػ�˵��󷵻���һ��������ѡ�����
 * \retval           RESULT_MENU_REDRAW �������ڲ�ʹ�õ���Ϣ�����¼��ز˵�������Ƕ�׵��� menulist ����
 * \retval           RESULT_REDRAW ���𼶣�ȡ������ֱ���˳��˵��ȷ���AP���ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \ingroup      controls
 * \par          exmaple code
 * \code
 menu_com_data_t menu_com;
 app_result_e result;

 menu_com.app_id = APP_ID_MUSIC;
 menu_com.menu_entry = MENU_MUSIC_PLAY;
 menu_com.path_id = PATH_ID_MUSIC_PLAY;
 menu_com.browse_mode = 0;//ֻ֧�ִ���ڲ˵���ʼ���
 menu_com.menu_func = NULL;
 result = gui_menulist_simple(MENULIST_OPTION, &menu_com);
 if(result == RESULT_REDRAW)//�˵��б������˳����ػ�UI
 {

 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 *******************************************************************************/
app_result_e gui_menulist_simple(uint16 style_id, menu_com_data_t *menu_com)
{
    menulist_control_t ml_control; //�˵��б���ƽṹ��
    menu_title_action_t title_action;
    style_infor_t style_infor;

    app_result_e result = RESULT_NULL;

    uint8 cur_menu_id; //��ǰ�˵�id
    int8 scroll_timer = -1;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_MENU_SIMPLE);

    gui_get_headbar_mode(&mode_backup, &icon_backup);

    //��ʼ���˵����ƽṹ��
    ml_control.style_id = style_id;
    ml_control.stack_pointer = 0;
    ml_control.draw_mode = LIST_DRAW_ALL;
    __init_action_title(&title_action);
    title_action.style_id = style_id;

    //ִ�иú���������ʵ��һЩ���⹦�ܣ������޸Ĳ˵������˵����ַ�����
    if (menu_com->menu_func != NULL)
    {
        menu_com->menu_func(&title_action);
    }

    cur_menu_id = menu_com->menu_entry;
    ml_control.enter_menu = TRUE;

    //����˵�ѭ��
    while (1)
    {
        if (ml_control.draw_mode != LIST_DRAW_NULL)//�����б�
        {
            //���ˣ�Ҫ�� menu_stack �� title_action Ϊ��ǰ��β˵�������Ϣ������ͻ����
            style_infor.style_id = style_id;
            style_infor.type = UI_COM;

            //���� listbox �ؼ����Բ���
            ui_get_listbox_attrb(&style_infor, &ml_control.one_page_count, 0);

            if (ml_control.enter_menu == TRUE)
            {
                //���ز˵�ͷ����ʼ�� ml_control
                __load_conf_title_simple(&ml_control, cur_menu_id);
            }

            //����һҳ�˵���
            __load_conf_item_simple(&ml_control);

            //��ʾ�˵��б�
            if (get_screensave_state() == FALSE)
            {
                if (ml_control.draw_mode == LIST_DRAW_ALL)
                {
                    if ((style_id == MENULIST) || (style_id == MENULIST_TITLE))
                    {
                        gui_set_headbar_mode(HEADBAR_MODE_NORMAL, this_headbar_icon_id);
                    }
                    else
                    {
                        gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);
                    }
                }

                update_menulist_private(&ml_control, &menu_private, &title_action);
                if (ui_show_listbox(&style_infor, &menu_private, (uint32) ml_control.draw_mode) == UI_NEED_SCROLL)
                {
                    if (scroll_timer == -1)//δ����������
                    {
                        scroll_timer = set_app_timer(APP_TIMER_ATTRB_UI, 80, __menulist_scroll_handle);
                    }
                }
                else
                {
                    //ɾ����ʱ������ʹû����Ҳû����
                    kill_app_timer(scroll_timer);
                    scroll_timer = -1;
                }
            }

            ml_control.enter_menu = FALSE;
            ml_control.draw_mode = LIST_DRAW_NULL;
        }

        result = menulist_simple_msg_deal(&ml_control, &title_action);
        if (result != RESULT_NULL)
        {
            break;
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    kill_app_timer(scroll_timer);
    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}

/*! \endcond */

/*! \cond */

//���ز˵�ͷ����ʼ�� ml_control
static void __load_conf_title_simple(menulist_control_t *p_ml_control, uint8 menu_id)
{
    //���ز˵�ͷ�ṹ��
    com_get_confmenu_title(menu_id, &g_menu_title);

    //��ʼ�� ml_control
    p_ml_control->select = 0;
    p_ml_control->list_no = 0;
    p_ml_control->top = 0;
    p_ml_control->total = g_menu_title.count;
    p_ml_control->bottom = p_ml_control->top + p_ml_control->one_page_count - 1;
    if (p_ml_control->bottom > (p_ml_control->total - 1))
    {
        p_ml_control->bottom = p_ml_control->total - 1;
    }
    p_ml_control->old = p_ml_control->active = 0;
}

//����һҳ�˵���
static void __load_conf_item_simple(menulist_control_t *p_ml_control)
{
    uint8 i;

    for (i = 0; i < p_ml_control->one_page_count; i++)
    {
        if ((i + p_ml_control->top) > p_ml_control->bottom)
        {
            break;
        }

        //��ȡ�˵����ַ���
        com_get_confmenu_item(&g_menu_title, p_ml_control->top + i, g_menu_items + i);
    }
}

static void __init_action_title(menu_title_action_t *title_action)
{
    libc_memset(title_action, 0xff, sizeof(menu_title_action_t));
    //title_action->style_id = V_U16_INVALID;
    //title_action->title_id = V_U16_INVALID;
    //title_action->str_id = V_U16_INVALID;
    //title_action->menu_entry = V_U16_INVALID;//ǿ��menu_entry����������
    //title_action->source_id = V_U16_INVALID;
    title_action->ret_str = NULL;
}

static void __menulist_scroll_handle(void)
{
    ui_show_listbox(NULL, NULL, LIST_DRAW_ACTIVE_SCROLL);
}

/*! \endcond */
