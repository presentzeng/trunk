/*******************************************************************************
 *                              us212A
 *                            Module: commonUI-menulist
 *                 Copyright(c) 2003-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-15 15:02     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_menulist_sub2.c
 * \brief    commonUI �˵��б�ؼ�������ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�ֿ����û��ģ���·�����书�ܵĲ˵��б�GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�listbox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2011-9-15
 *******************************************************************************/

#include "common_ui.h"

#ifdef PC
extern __get_menu_func this_get_menu_func;
extern __get_callback_func this_get_callback_func;
extern __get_menu_option this_get_menu_option;
extern void load_menulist_simfunc(__get_menu_func func1, __get_callback_func func2, __get_menu_option func3);
#endif

extern void menulist_select_next(menulist_control_t *p_ml_control)__FAR__;
extern void menulist_select_prev(menulist_control_t *p_ml_control)__FAR__;
extern void write_history_item_menu(menulist_control_t *p_ml_control, bool update)__FAR__;
extern void load_conf_title(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack)__FAR__;
extern void load_confmenu_title_data(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)__FAR__;
extern void re_enter_currentdir(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action, menu_cb_func menu_func)__FAR__;
extern void re_redraw_currentdir(menulist_control_t *p_ml_control, menu_title_action_t *title_action)__FAR__;

/*! \cond */

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

//���� menu stack ���Ʊ���
static void update_menu_stack(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action, uint16 ori_menu_entry, uint8 mode)
{
    if (mode == 0)//���붯̬��ڲ˵�
    {
        p_menu_stack[p_ml_control->stack_pointer].cur_entry = (uint8) title_action->menu_entry;
        p_menu_stack[p_ml_control->stack_pointer].ori_entry = (uint8) ori_menu_entry;//old�˵�ͷID
        p_menu_stack[p_ml_control->stack_pointer].menu_id = (uint8) title_action->menu_entry;
        p_menu_stack[p_ml_control->stack_pointer].layer = 0;
    }
    else//������һ��ʱ�滻Ϊ��̬��ڲ˵���ֻ�������ײ�
    {
        //��̬��ڲ˵��������µ���ڲ˵����滻���ɵ���ڲ˵�
        p_menu_stack[p_ml_control->stack_pointer].cur_entry = (uint8) title_action->menu_entry;
        p_menu_stack[p_ml_control->stack_pointer].ori_entry = (uint8) title_action->menu_entry;
        p_menu_stack[p_ml_control->stack_pointer].menu_id = (uint8) title_action->menu_entry;
        //�㼶���䣬�������
    }
}

//menulist gui app msg deal--gui
app_result_e menulist_msg_deal_gui(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action, msg_apps_type_e gui_event)
{
    app_result_e result = RESULT_NULL;
    menu_item_data_t *active_menu;
    menu_title_action_t temp_title_action;//��ʱ����
    bool deal_menu_result, deal_return_parent;

#ifdef PC
    menu_cb_func menu_func;
    menu_cb_option option_func;
#endif

    deal_menu_result = FALSE;
    deal_return_parent = FALSE;

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
        //���浱ǰ��β˵���¼����
        write_history_item_menu(p_ml_control, FALSE);
        active_menu = &g_menu_items[p_ml_control->active];
        //����Ҷ�Ӳ˵�����
        if ((active_menu->child_index & MENU_TYPE_LEAF) == MENU_TYPE_LEAF)
        {
            __init_action_title(&temp_title_action);//��գ������Ų���Ӱ��menu_func���
#ifndef PC
            result = active_menu->menu_func(&temp_title_action);
#else
            menu_func = this_get_menu_func(active_menu->str_id, active_menu->str_id_sub);
            result = menu_func(&temp_title_action);
#endif
            //Ҷ�Ӳ˵�ִ�з���ֵ����
            deal_menu_result = TRUE;
        }
        //������˵�����
        else
        {
            //��ȡ�¼��˵��Ĳ˵�ͷ�ͻ��ֵ
            if (active_menu->menu_func != NULL)
            {
                __init_action_title(&temp_title_action);//��գ������Ų���Ӱ��menu_func���
                //���ݸ��˵���ȷ������ȷ������ȷ�������ķ���ֵ���������� RESULT_NULL ����
#ifndef PC
                result = active_menu->menu_func(&temp_title_action);
#else
                menu_func = this_get_menu_func(active_menu->str_id, active_menu->str_id_sub);
                result = menu_func(&temp_title_action);
#endif
                //��Ҷ�Ӳ˵�����˽�����һ���⣬�����Ե���Ҷ�Ӳ˵�ִ�У�
                //�������ֵ���� RESULE_MENU_SON������Ϊ�ǵ���Ҷ�Ӳ˵�ִ��
                if (result != RESULT_MENU_SON)
                {
                    deal_menu_result = TRUE;

                    break;//case ��֧�Ӵ˴��˳�
                }

                libc_memcpy(title_action, &temp_title_action, sizeof(menu_title_action_t));
                if (title_action->menu_entry != V_U16_INVALID)
                {
                    //��������ڲ˵�
                    p_ml_control->stack_pointer++;
                    p_ml_control->layer_no++;

                    //��������ڲ˵������� menu stack ���Ʊ���
                    update_menu_stack(p_ml_control, p_menu_stack, title_action, active_menu->child_index, 0);
                }
                else
                {
                    //������һ���˵�
                    p_menu_stack[p_ml_control->stack_pointer].menu_id = (uint8) active_menu->child_index;
                    p_menu_stack[p_ml_control->stack_pointer].layer++;
                    p_ml_control->layer_no++;
                }
            }
            else
            {
                __init_action_title(title_action);//��գ�����������һ���˵�
                if (title_action->menu_entry == V_U16_INVALID)
                {
                    //������һ���˵�
                    p_menu_stack[p_ml_control->stack_pointer].menu_id = (uint8) active_menu->child_index;
                    p_menu_stack[p_ml_control->stack_pointer].layer++;
                    p_ml_control->layer_no++;
                }
            }

            //���¼��ز˵��б�
            p_ml_control->enter_menu = TRUE;
            p_ml_control->draw_mode = LIST_DRAW_LIST;
            result = RESULT_NULL;//���˳��ؼ�
        }
        break;

        case EVENT_MENULIST_BACK_PARENT:
        //ִ�з��ظ��˵��ص�����
        if ((g_back_to_parant_arg != NULL) && (*g_back_to_parant_arg != 0))
        {
            g_back_to_parent_func();
            g_back_to_parant_arg = NULL;
        }

        //���浱ǰ��β˵���¼����
        write_history_item_menu(p_ml_control, FALSE);
        __init_action_title(title_action);//��գ������Ų���Ӱ���´δ���
        result = RESULT_NULL;
        deal_return_parent = TRUE;
        break;

        case EVENT_MENULIST_ENTER_OPTION:
        //���浱ǰ��β˵���¼����
        write_history_item_menu(p_ml_control, FALSE);
        active_menu = &g_menu_items[p_ml_control->active];
        if (active_menu->menu_option != NULL)
        {
#ifndef PC
            result = active_menu->menu_option();
#else
            option_func = this_get_menu_option(active_menu->str_id, active_menu->str_id_sub);
            result = option_func();
#endif
            switch (result)
            {
                case RESULT_MENU_REDRAW://���¼��ز˵�������Ƕ�׵��� menulist ����
                p_ml_control->enter_menu = TRUE;
                p_ml_control->draw_mode = LIST_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
                break;

                case RESULT_MENU_PARENT:
                __init_action_title(title_action);//��գ�������ֻ�ᰴ������ͨ��ʽ������һ��
                deal_return_parent = TRUE;
                break;

                case RESULT_REDRAW:
                p_ml_control->draw_mode = LIST_DRAW_ALL;
                result = RESULT_NULL;//���˳��ؼ�
                break;

                default:
                //do nothing
                break;
            }
        }
        break;

        default:
        result = com_message_box(gui_event);
        if (result == RESULT_REDRAW)
        {
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
        break;
    }

    //�˵��ص���������ֵ����
    if (deal_menu_result == TRUE)
    {
        switch (result)
        {
            case RESULT_MENU_EXIT:
            result = RESULT_REDRAW;
            break;

            case RESULT_MENU_PARENT:
            libc_memcpy(title_action, &temp_title_action, sizeof(menu_title_action_t));
            deal_return_parent = TRUE;
            break;

            case RESULT_MENU_CUR:
            //���½��뵱ǰ��˵���Ϊ�˸���Ϊ��style�����
            libc_memcpy(title_action, &temp_title_action, sizeof(menu_title_action_t));
            re_enter_currentdir(p_ml_control, p_menu_stack, title_action, p_ml_control->menu_com->menu_func);
            //���¼��ز˵��б�
            p_ml_control->enter_menu = TRUE;
            p_ml_control->draw_mode = LIST_DRAW_ALL;
            result = RESULT_NULL;//���˳��ؼ�
            break;

            case RESULT_MENU_REDRAW_PARENT:
            //�ػ��󷵻���һ��
            re_redraw_currentdir(p_ml_control, &temp_title_action);
            __init_action_title(title_action);//��գ���Ĭ�Ϸ�ʽ������һ��
            deal_return_parent = TRUE;
            break;

            case RESULT_REDRAW:
            p_ml_control->draw_mode = LIST_DRAW_ALL;
            result = RESULT_NULL;//���˳��ؼ�
            break;

            default:
            //do nothing
            break;
        }
    }

    //������һ���˵�����
    if (deal_return_parent == TRUE)
    {
        if (g_menu_title.father_index != MENU_TYPE_ENTRY)
        {
            if (p_menu_stack[p_ml_control->stack_pointer].layer == 0)
            {
                //�˳���ǰ��ڲ˵���������һ����ڲ˵�
                p_ml_control->stack_pointer--;
                p_ml_control->layer_no--;
            }
            else
            {
                //������һ���˵�
                p_menu_stack[p_ml_control->stack_pointer].menu_id = (uint8) g_menu_title.father_index;
                p_menu_stack[p_ml_control->stack_pointer].layer--;
                p_ml_control->layer_no--;
            }

            //��ȡ�˵�ͷ�ṹ�壬������ re_enter_currentdir ���ж��Ƿ񵽴�˵����
            load_conf_title(p_ml_control, p_menu_stack);

            //���½��뵱ǰ��˵���Ҳ����˵��������һ����ͬ���ȼ򵥷�����һ�������½��뵱ǰ��˵�
            re_enter_currentdir(p_ml_control, p_menu_stack, title_action, p_ml_control->menu_com->menu_func);
            //���¼��ز˵��б�
            p_ml_control->enter_menu = TRUE;
            if (result == RESULT_NULL)
            {
                p_ml_control->draw_mode = LIST_DRAW_LIST;
            }
            else//������ʽ������һ������Ҫȫ���ػ�
            {
                p_ml_control->draw_mode = LIST_DRAW_ALL;
            }

            result = RESULT_NULL;//���˳��ؼ�
        }
        else
        {
            //����ڲ˵��˳��˵�������
            result = RESULT_REDRAW;
        }
    }

    return result;
}

/*! \endcond */
