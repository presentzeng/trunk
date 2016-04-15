/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-15 15:02     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_menulist.c
 * \brief    gui ���������˵��б�ؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�ֿ����û��ģ���·�����书�ܵĲ˵��б�ؼ���
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��ui������applib��common_func���ȵȡ�
 * \version  1.0
 * \date     2011-9-15
 *******************************************************************************/

#include "common_ui.h"

/*! \cond COMMON_API */

static listbox_private_t menu_private _BANK_DATA_ATTR_;
static void __menulist_scroll_handle(void);
void load_confmenu_item_data(menulist_control_t *p_ml_control);

#ifdef PC
__get_menu_func this_get_menu_func;
__get_callback_func this_get_callback_func;
__get_menu_option this_get_menu_option;

void load_menulist_simfunc(__get_menu_func func1, __get_callback_func func2, __get_menu_option func3)
{
    this_get_menu_func = func1;
    this_get_callback_func = func2;
    this_get_menu_option = func3;
}
#endif

extern void update_menulist_private(menulist_control_t *p_ml_control, listbox_private_t *menu_private,
        menu_title_action_t *title_action)__FAR__;
extern void write_history_item_menu(menulist_control_t *p_ml_control, bool update)__FAR__;
extern void load_conf_title(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack)__FAR__;
extern void load_confmenu_title_data(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)__FAR__;
extern void enter_newest_menulist(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)__FAR__;
extern void re_enter_currentdir(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action, menu_cb_func menu_func)__FAR__;
extern void init_menulist_param(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)__FAR__;

extern app_result_e menulist_msg_deal_gui(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action, msg_apps_type_e gui_event) __FAR__;
extern app_result_e menulist_msg_deal(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)__FAR__;

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
 menu_com.browse_mode = 0;//����ڲ˵���ʼ�����Ϊ1��ʾ�������˳�����ʼ���
 menu_com.menu_func = NULL;
 result = gui_menulist(MENULIST, &menu_com);
 if(result == RESULT_REDRAW)//�˵��б������˳����ػ�UI
 {

 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 *******************************************************************************/
app_result_e gui_menulist(uint16 style_id, menu_com_data_t *menu_com)
{
    menulist_control_t ml_control; //�˵��б���ƽṹ��
    menulist_stack_t menu_stack[3]; //�˵�Ƕ�׿���
    menu_title_action_t title_action;
    style_infor_t style_infor;

#ifdef PC
    menu_cb_leaf callback_func;
#endif

    app_result_e result = RESULT_NULL;

    uint16 last_style; //��һ���˵� style id
    int8 scroll_timer = -1;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_MENU);

    gui_get_headbar_mode(&mode_backup, &icon_backup);

    //��ȡ�˵�����·������ʼ���˵����ƽṹ��
    menu_history_valid = FALSE;
    last_style = style_id;
    ml_control.style_id = style_id;
    ml_control.menu_com = menu_com;
    init_menulist_param(&ml_control, menu_stack, &title_action);
    //��ȡ�˵�ͷ�ṹ�壬������ re_enter_currentdir ���ж��Ƿ񵽴�˵����
    load_conf_title(&ml_control, menu_stack);
    //�������˵�ʱ����ĳЩ���⴦����ͬ�����½�����ڲ˵���title_action.str_idָ��select�
    re_enter_currentdir(&ml_control, menu_stack, &title_action, menu_com->menu_func);//��ʱtitle_action.str_idΪ-1
    //��Ҫ�������²˵��˳�·��
    if (menu_com->browse_mode == 1)
    {
        //�������������²˵��˳�·��
        enter_newest_menulist(&ml_control, menu_stack, &title_action);
    }
    ml_control.enter_menu = TRUE;

    //����˵�ѭ��
    while (1)
    {
        if (ml_control.draw_mode != LIST_DRAW_NULL)//�����б�
        {
            //���ˣ�Ҫ�� menu_stack �� title_action Ϊ��ǰ��β˵�������Ϣ������ͻ����
            if (title_action.style_id == V_U16_INVALID)
            {
                style_infor.style_id = style_id;
            }
            else
            {
                style_infor.style_id = title_action.style_id;
            }
            style_infor.type = UI_COM;
            if (style_infor.style_id != last_style)
            {
                ml_control.draw_mode = LIST_DRAW_ALL;
            }
            last_style = style_infor.style_id;

            //���� listbox �ؼ����Բ���
            ui_get_listbox_attrb(&style_infor, &ml_control.one_page_count, 0);

            if (ml_control.enter_menu == TRUE)
            {
                //���ز˵�ͷ����ʼ�� ml_control
                load_confmenu_title_data(&ml_control, menu_stack, &title_action);
            }

            //����һҳ�˵���
            load_confmenu_item_data(&ml_control);

            //��ʾ�˵��б�
            if (get_screensave_state() == FALSE)
            {
                if (ml_control.draw_mode == LIST_DRAW_ALL)
                {
                    ui_set_backgd_color(Color_BACKCOLOR_def);//��Ϊȫ���� //2015-10-30
                    ui_clear_screen(NULL);

                    if ((last_style == MENULIST) || (last_style == MENULIST_TITLE))
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

            //��ʱҶ�Ӳ˵���Ӧ
            if ((ml_control.enter_menu == FALSE) && (g_menu_items[ml_control.active].callback != NULL))
            {
#ifndef PC
                g_menu_items[ml_control.active].callback();
#else
                        callback_func = this_get_callback_func(g_menu_items[ml_control.active].str_id, g_menu_items[ml_control.active].str_id_sub);
                        callback_func();
#endif
                    }

                    ml_control.enter_menu = FALSE;
                    ml_control.draw_mode = LIST_DRAW_NULL;
                }

                result = menulist_msg_deal(&ml_control, menu_stack, &title_action);
                if(result != RESULT_NULL)
                {
                    break;
                }

                //����10ms�����������
                        sys_os_time_dly(1);
                    }

                    write_history_item_menu(&ml_control, TRUE);
                    kill_app_timer(scroll_timer);
                    change_app_timer_tag(tag_backup);
                    gui_set_headbar_mode(mode_backup, icon_backup);
                    return result;
                }

                /*! \endcond */

                /*! \cond */

static void __menulist_scroll_handle(void)
{
    ui_show_listbox(NULL, NULL, LIST_DRAW_ACTIVE_SCROLL);
}

//menulist gui app msg deal
app_result_e menulist_msg_deal(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)
{
    input_gui_msg_t input_msg; //��������Ϣ����gui��Ϣ
    msg_apps_type_e gui_event; //gui ��Ϣӳ���¼�
    private_msg_t private_msg; //˽����Ϣ
    app_result_e result = RESULT_NULL;
    
    //��ȡgui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)//��gui��Ϣ
    {
        //���а���ӳ��
        if (com_key_mapping(&input_msg, &gui_event, menulist_key_map_list) == TRUE)
        {
            result = menulist_msg_deal_gui(p_ml_control, p_menu_stack, title_action, gui_event);
        }
    }
    else
    {
        //���ˣ�gui��Ϣ�����Ѿ��������
        //����ap˽����Ϣ��ϵͳ��Ϣ
        if (get_app_msg(&private_msg) == TRUE)
        {
            result = g_this_app_msg_dispatch(&private_msg);
            //���β��ˢ��MENU(ap_music��ڲ˵�-- ���и���������Ŀ¼)
            if (g_this_app_info->app_id == APP_ID_MUSIC)
            {
                if (((p_ml_control->menu_com->menu_entry == 0) || (p_ml_control->menu_com->menu_entry == 1))
                        && (p_ml_control->layer_no == 0))
                {
                    if ((private_msg.msg.type == MSG_SD_IN) || (private_msg.msg.type == MSG_SD_OUT))
                    {
                        result = RESULT_REFRESH_MENU;
                    }
                }
            }
            if (result == RESULT_REDRAW)
            {
                //������ʾ��ʾ�������ػ�UI
                p_ml_control->draw_mode = LIST_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
            }
            else if (result != RESULT_NULL)
            {
                //ִ�з��ظ��˵��ص�����
                if ((g_back_to_parant_arg != NULL) && (*g_back_to_parant_arg != 0))
                {
                    g_back_to_parent_func();
                    g_back_to_parant_arg = NULL;
                }
            }
            else
            {
            }
        }
    }

    return result;
}

//����һҳ�˵���
void load_confmenu_item_data(menulist_control_t *p_ml_control)
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

/*! \endcond */
