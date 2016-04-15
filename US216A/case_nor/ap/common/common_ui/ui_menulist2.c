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
 * \file     _ui_menulist2.c
 * \brief    commonUI �˵��б�ؼ��ؼ�·���Ӻ���ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ�ֿ����û��ģ���·�����书�ܵĲ˵��б�GUI��
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�listbox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2011-9-15
 *******************************************************************************/

#include "common_ui.h"

static listbox_private_t menu_private _BANK_DATA_ATTR_;

void init_ml_control(menulist_control_t *p_ml_control, uint16 str_id);
void read_history_item_menu(menulist_control_t *p_ml_control);
void read_next_layer_history(menulist_control_t *p_ml_control, history_item_t *history_item);
void write_history_item_menu(menulist_control_t *p_ml_control, bool update);

extern void update_menulist_private(menulist_control_t *p_ml_control, listbox_private_t *menu_private,
        menu_title_action_t *title_action)__FAR__;

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

//��ʼ���˵��б����
void init_menulist_param(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)
{
    p_ml_control->stack_pointer = 0;
    p_ml_control->path_id = p_ml_control->menu_com->path_id;
    p_ml_control->layer_no = 0;
    p_ml_control->draw_mode = LIST_DRAW_ALL;
    p_menu_stack[p_ml_control->stack_pointer].cur_entry = p_ml_control->menu_com->menu_entry;
    p_menu_stack[p_ml_control->stack_pointer].ori_entry = p_ml_control->menu_com->menu_entry;
    p_menu_stack[p_ml_control->stack_pointer].menu_id = p_ml_control->menu_com->menu_entry;
    p_menu_stack[p_ml_control->stack_pointer].layer = 0;
    __init_action_title(title_action);
    title_action->style_id = p_ml_control->style_id;
}

//���ز˵�ͷ�ṹ��
void load_conf_title(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack)
{
    com_get_confmenu_title(p_menu_stack[p_ml_control->stack_pointer].menu_id, &g_menu_title);
    //��ڲ˵��������Ƕ�̬��ڲ˵�����ô��Ҫ���� father_index �� father_active
    if ((p_menu_stack[p_ml_control->stack_pointer].layer == 0) && (p_menu_stack[p_ml_control->stack_pointer].menu_id
            != p_menu_stack[p_ml_control->stack_pointer].ori_entry))
    {
        menu_title_data_t temp_title;

        com_get_confmenu_title(p_menu_stack[p_ml_control->stack_pointer].ori_entry, &temp_title);
        g_menu_title.father_index = temp_title.father_index;
        g_menu_title.father_active = temp_title.father_active;
    }
}

//���ز˵�ͷ����ʼ�� ml_control
void load_confmenu_title_data(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)
{
    //���ز˵�ͷ�ṹ��
    load_conf_title(p_ml_control, p_menu_stack);

    //��ʼ�� ml_control
    init_ml_control(p_ml_control, title_action->str_id);
}

//����������˵������˳�·����������뵽�ϴ��˳�ʱ���Ӳ˵�һ��
void enter_newest_menulist(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action)
{
    menu_item_data_t *active_menu;
    history_item_t history_item;

    //ѭ��ģ�������һ���˵���ֱ��Ҷ�Ӳ˵�
    while (1)
    {
        //���ز˵�ͷ����ʼ�� ml_control
        p_ml_control->one_page_count = LIST_NUM_ONE_PAGE_MAX;//ΪĬ��6���
        load_confmenu_title_data(p_ml_control, p_menu_stack, title_action);

        //��ȡ��һ��·�����䣬�ж��Ƿ���ײ�·������
        read_next_layer_history(p_ml_control, &history_item);
        if (history_item.top == 0xffff)//��Ч·��������
        {
            break;//�ѵ���ײ�·�����䣬�˳�
        }

        //��ȡactive ����浽 g_menu_items[0]
        com_get_confmenu_item(&g_menu_title, p_ml_control->list_no, g_menu_items);
        active_menu = &g_menu_items[0];
        if ((active_menu->child_index & MENU_TYPE_LEAF) == MENU_TYPE_LEAF)
        {
            break;//�ѵ�Ҷ�Ӳ˵����˳�
        }

        __init_action_title(title_action);
        //�Զ�ѡ�� active(��Ӧlist_no) �����
        if (active_menu->menu_func != NULL)
        {
#ifdef PC
            menu_cb_func func;
#endif

            //���ݸ��˵���ȷ������ȷ������ȷ�������ķ���ֵ���������� RESULT_NULL ����
#ifndef PC
            active_menu->menu_func(title_action);
#else
            func = this_get_menu_func(active_menu->str_id, active_menu->str_id_sub);
            func(title_action);
#endif
            if (title_action->menu_entry != V_U16_INVALID)
            {
                //��������ڲ˵�
                p_ml_control->stack_pointer++;
                p_ml_control->layer_no++;

                //��������ڲ˵������� menu stack ���Ʊ���
                update_menu_stack(p_ml_control, p_menu_stack, title_action, active_menu->child_index, 0);
            }
        }

        if (title_action->menu_entry == V_U16_INVALID)
        {
            //������һ���˵�
            p_menu_stack[p_ml_control->stack_pointer].menu_id = (uint8) active_menu->child_index;
            p_menu_stack[p_ml_control->stack_pointer].layer++;
            p_ml_control->layer_no++;
        }
    }
}

//���½��뵱ǰ��˵������ڷ�����һ�������½��룬Ϊ�˸���Ϊ��style�����
//���������ùػ�ʱ��󷵻أ���Ҫ����Ϊ��style����ڣ��Ա�鿴���ý����
void re_enter_currentdir(menulist_control_t *p_ml_control, menulist_stack_t *p_menu_stack,
        menu_title_action_t *title_action, menu_cb_func menu_func)
{
    menu_item_data_t *active_menu;
    menu_title_action_t temp_title_action;
    uint16 str_id_bk = title_action->str_id;

    __init_action_title(&temp_title_action);
    //������㣬ֱ�Ӽ�����ڲ˵�
    if ((p_ml_control->stack_pointer == 0) && (p_menu_stack[0].layer == 0))
    {
        //��ʱ�п�����Ҫ���������ڲ˵���Ҫ���� menu_func �д���
        if (menu_func != NULL)
        {
            menu_func(title_action);
            if (title_action->menu_entry != V_U16_INVALID)
            {
                //�滻��ڲ˵������� menu stack ���Ʊ���
                update_menu_stack(p_ml_control, p_menu_stack, title_action, 0, 1);
            }
        }
    }
    else//��δ����㣬������һ��
    {
        //��ʱ���п����Ƿ�����Ϊ��̬�˵�����һ����������Ϊ�ö�̬�˵��ڵ��
        //ԭ�˵��ڵ��ڲ˵������ǵȼ۵ģ����Կ���ֱ�Ӽ���������һ��
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

        //���ز˵�ͷ����ʼ�� ml_control
        p_ml_control->one_page_count = LIST_NUM_ONE_PAGE_MAX;//ΪĬ��6���
        load_confmenu_title_data(p_ml_control, p_menu_stack, &temp_title_action);

        //��ȡactive ����浽 g_menu_items[0]
        com_get_confmenu_item(&g_menu_title, p_ml_control->list_no, g_menu_items);
        active_menu = &g_menu_items[0];

        //���½���
        __init_action_title(title_action);
        if (active_menu->menu_func != NULL)
        {
#ifdef PC
            menu_cb_func func;
#endif

            //���ݸ��˵���ȷ������ȷ������ȷ�������ķ���ֵ���������� RESULT_NULL ����
#ifndef PC
            active_menu->menu_func(title_action);
#else
            func = this_get_menu_func(active_menu->str_id, active_menu->str_id_sub);
            func(title_action);
#endif
            if (title_action->menu_entry != V_U16_INVALID)
            {
                //��������ڲ˵�
                p_ml_control->stack_pointer++;
                p_ml_control->layer_no++;

                //��������ڲ˵������� menu stack ���Ʊ���
                update_menu_stack(p_ml_control, p_menu_stack, title_action, active_menu->child_index, 0);
            }
        }

        if (title_action->menu_entry == V_U16_INVALID)
        {
            //������һ���˵�
            p_menu_stack[p_ml_control->stack_pointer].menu_id = (uint8) active_menu->child_index;
            p_menu_stack[p_ml_control->stack_pointer].layer++;
            p_ml_control->layer_no++;
        }
    }

    if (str_id_bk != V_U16_INVALID)
    {
        title_action->str_id = str_id_bk;
    }
}

//�ػ浱ǰ�˵���ͣ��0.5���ӷ���
void re_redraw_currentdir(menulist_control_t *p_ml_control, menu_title_action_t *title_action)
{
    style_infor_t style_infor;

    if (title_action->style_id == V_U16_INVALID)
    {
        style_infor.style_id = p_ml_control->style_id;
    }
    else
    {
        style_infor.style_id = title_action->style_id;
    }
    style_infor.type = UI_COM;
    p_ml_control->draw_mode = LIST_DRAW_ALL;

    //���� select ��
    p_ml_control->select = com_get_confmenu_active(&g_menu_title, title_action->str_id);

    //��ʾ�˵��б�
    update_menulist_private(p_ml_control, &menu_private, title_action);
    ui_show_listbox(&style_infor, &menu_private, (uint32) p_ml_control->draw_mode);

    if ((style_infor.style_id == MENULIST) || (style_infor.style_id == MENULIST_TITLE))
    {
        //ǿ��ˢ��״̬��
        gui_headbar(HEADBAR_UPDATE_ALL);
    }

    //�ȴ�0.5���Ӻ󷵻�
    sys_os_time_dly(50);
}

//����·����ʷ��¼
void write_history_item_menu(menulist_control_t *p_ml_control, bool update)
{
    history_index_t temp_index;
    history_item_t temp_history_item;

    temp_history_item.top = p_ml_control->top;
    temp_history_item.list_no = p_ml_control->list_no;

    temp_index.type = PATH_HISTORY_MENU;
    temp_index.app_id = g_this_app_info->app_id;
    temp_index.path_id = p_ml_control->path_id;
    temp_index.layer = p_ml_control->layer_no;
    if (update == FALSE)
    {
        com_set_history_item(&temp_index, &temp_history_item);
    }
    else
    {
        com_update_path_history(&temp_index);
    }
}

//��ȡ·����ʷ��¼
void read_history_item_menu(menulist_control_t *p_ml_control)
{
    history_index_t temp_index;
    history_item_t temp_history_item;

    temp_index.type = PATH_HISTORY_MENU;
    temp_index.app_id = g_this_app_info->app_id;
    temp_index.path_id = p_ml_control->path_id;
    temp_index.layer = p_ml_control->layer_no;
    com_get_history_item(&temp_index, &temp_history_item);

    if (temp_history_item.top == V_U16_INVALID)
    {
        p_ml_control->top = 0;
        p_ml_control->list_no = 0;
    }
    else if ((temp_history_item.top >= p_ml_control->total) || (temp_history_item.list_no >= p_ml_control->total)
            || (temp_history_item.top > temp_history_item.list_no) || ((temp_history_item.list_no
            - temp_history_item.top) > (p_ml_control->one_page_count - 1)))
    {
        //·�������������ǿ��ָ��ͷ��������д��·�����䣬��ǿ�ưѺ���㼶��Ϊ��Ч
        p_ml_control->top = 0;
        p_ml_control->list_no = 0;
        write_history_item_menu(p_ml_control, FALSE);
    }
    else
    {
        p_ml_control->top = temp_history_item.top;
        p_ml_control->list_no = temp_history_item.list_no;
    }
}

//��ȡ��һ��·����ʷ��¼
void read_next_layer_history(menulist_control_t *p_ml_control, history_item_t *history_item)
{
    history_index_t temp_index;

    temp_index.type = PATH_HISTORY_MENU;
    temp_index.app_id = g_this_app_info->app_id;
    temp_index.path_id = p_ml_control->path_id;
    temp_index.layer = p_ml_control->layer_no + 1;
    com_get_history_item(&temp_index, history_item);
}

//��ʼ��menulist_control_t���ݽṹ
void init_ml_control(menulist_control_t *p_ml_control, uint16 str_id)
{
    uint16 select_no;

    //����У��˵�·�������Ƿ���Ч
    p_ml_control->total = g_menu_title.count;

    //��ʼ�� p_ml_control total top list_no select ����
    if (str_id != V_U16_INVALID)//ͨ���ַ���IDƥ��˵�������
    {
        //ƥ��˵��������������ȷ��ڵ����ڶ���
        select_no = com_get_confmenu_active(&g_menu_title, str_id);
        if (select_no != V_U16_INVALID)
        {
            //ƥ�䵽���������ƥ������ʼ��
            p_ml_control->select = select_no;
            p_ml_control->list_no = select_no;
            p_ml_control->top = 0;
        }
        else
        {
            //����ƥ�䵽�������ȡ��ǰ��β˵���¼����
            read_history_item_menu(p_ml_control);
            p_ml_control->select = V_U16_INVALID;
        }
    }
    else
    {
        //��ȡ��ǰ��β˵���¼����
        read_history_item_menu(p_ml_control);
        p_ml_control->select = V_U16_INVALID;
    }

    //�����ü�����ڵ����ڶ���
    if (p_ml_control->total > p_ml_control->one_page_count)
    {
        if (p_ml_control->list_no == (p_ml_control->total - 1))//���һ�ֻ��ͣ�ڵ�ǰҳ���һ��
        {
            p_ml_control->top = p_ml_control->list_no - (p_ml_control->one_page_count - 1);
        }
        else if (p_ml_control->list_no >= (p_ml_control->one_page_count - 2))//�������һ�����ͣ�ڵ�ǰҳ�����ڶ���
        {
            p_ml_control->top = p_ml_control->list_no - (p_ml_control->one_page_count - 2);
        }
        else
        {
            p_ml_control->top = 0;
        }
    }
    else
    {
        p_ml_control->top = 0;
    }

    //��ʼ�� p_ml_control ��������
    p_ml_control->bottom = p_ml_control->top + p_ml_control->one_page_count - 1;
    if (p_ml_control->bottom > (p_ml_control->total - 1))
    {
        p_ml_control->bottom = p_ml_control->total - 1;
    }
    p_ml_control->old = p_ml_control->active = p_ml_control->list_no - p_ml_control->top;
}
