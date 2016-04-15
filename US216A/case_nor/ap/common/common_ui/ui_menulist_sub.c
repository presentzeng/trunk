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
 * \file     _ui_menulist_sub.c
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

/*! \cond */

/******************************************************************************/
/*!
 * \par  Description:
 *    ѡ��˵��б��е���һ��
 * \param[in]    p_ml_control��ָ��˵��б���ƽṹ��
 * \param[out]   p_ml_control�����ظ����˵Ĳ˵��б����
 * \return       none
 * \note
 *******************************************************************************/
void menulist_select_next(menulist_control_t *p_ml_control)
{
    //��ҳ���л����к���δ��ҳ�����һ���ൽ�����ڶ�������¼�����
    if ((p_ml_control->list_no + 1) < p_ml_control->bottom)
    {
        p_ml_control->list_no++;
        p_ml_control->old = p_ml_control->active;
        p_ml_control->active++;
        p_ml_control->draw_mode = LIST_DRAW_ACTIVE;
    }
    else
    {
        //���б����һ������б�ͷ��ʾ
        if (p_ml_control->list_no == (p_ml_control->total - 1))
        {
            p_ml_control->list_no = 0;
            //����һҳ������£�ֻ����¼�����
            if (p_ml_control->total <= p_ml_control->one_page_count)
            {
                //����������1�����Ҫ�л�
                if (p_ml_control->total > 1)
                {
                    p_ml_control->old = p_ml_control->active;
                    p_ml_control->active = 0;
                    p_ml_control->draw_mode = LIST_DRAW_ACTIVE;
                }
            }
            //��β�л���ͷ�����������б�
            else
            {
                p_ml_control->top = 0;
                p_ml_control->bottom = p_ml_control->one_page_count - 1;
                p_ml_control->old = p_ml_control->active = 0;
                p_ml_control->update_mode = LIST_UPDATE_HEAD;
                p_ml_control->draw_mode = LIST_DRAW_LIST;
            }
        }
        else
        {
            //���������еĵ����ڶ���е����һ�ֻ����¼�����
            if (p_ml_control->list_no == (p_ml_control->total - 2))
            {
                p_ml_control->list_no++;
                p_ml_control->old = p_ml_control->active;
                p_ml_control->active++;
                p_ml_control->draw_mode = LIST_DRAW_ACTIVE;
            }
            //�ڵ�ǰҳ�еĵ����ڶ�����Һ������㹻�б��������ҳ�����������б�
            else
            {
                p_ml_control->list_no++;
                p_ml_control->top++;
                p_ml_control->bottom++;
                p_ml_control->old = p_ml_control->active;
                p_ml_control->update_mode = LIST_UPDATE_NEXT;
                p_ml_control->draw_mode = LIST_DRAW_LIST;
            }
        }
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ѡ��˵��б��е���һ��
 * \param[in]    p_ml_control��ָ��˵��б���ƽṹ��
 * \param[out]   p_ml_control�����ظ����˵Ĳ˵��б����
 * \return       none
 * \note
 *******************************************************************************/
void menulist_select_prev(menulist_control_t *p_ml_control)
{
    //��ҳ���л����к���δ��ҳ�������ൽ�ڶ�������¼�����
    if (p_ml_control->list_no > (p_ml_control->top + 1))
    {
        p_ml_control->list_no--;
        p_ml_control->old = p_ml_control->active;
        p_ml_control->active--;
        p_ml_control->draw_mode = LIST_DRAW_ACTIVE;
    }
    else
    {
        //���б��һ������б�β��ʾ
        if (p_ml_control->list_no == 0)
        {
            p_ml_control->list_no = p_ml_control->total - 1;
            //����һҳ������£�ֻ����¼�����
            if (p_ml_control->total <= p_ml_control->one_page_count)
            {
                //����������1�����Ҫ�л�
                if (p_ml_control->total > 1)
                {
                    p_ml_control->old = p_ml_control->active;
                    p_ml_control->active = p_ml_control->total - 1;
                    p_ml_control->draw_mode = LIST_DRAW_ACTIVE;
                }
            }
            //��ͷ�е�β�����������б�
            else
            {
                p_ml_control->top = p_ml_control->total - p_ml_control->one_page_count;
                p_ml_control->bottom = p_ml_control->list_no;
                p_ml_control->old = p_ml_control->active = p_ml_control->one_page_count - 1;
                p_ml_control->update_mode = LIST_UPDATE_TAIL;
                p_ml_control->draw_mode = LIST_DRAW_LIST;
            }
        }
        else
        {
            //���������еĵڶ���е���һ�ֻ����¼�����
            if (p_ml_control->list_no == 1)
            {
                p_ml_control->list_no--;
                p_ml_control->old = p_ml_control->active;
                p_ml_control->active--;
                p_ml_control->draw_mode = LIST_DRAW_ACTIVE;
            }
            //�ڵ�ǰҳ�еĵڶ������ǰ�����㹻�б��������ҳ�����������б�
            else
            {
                p_ml_control->list_no--;
                p_ml_control->top--;
                p_ml_control->bottom--;
                p_ml_control->old = p_ml_control->active;
                p_ml_control->update_mode = LIST_UPDATE_PREV;
                p_ml_control->draw_mode = LIST_DRAW_LIST;
            }
        }
    }
}

//ͬ������listbox˽�����ݽṹ
void update_menulist_private(menulist_control_t *p_ml_control, listbox_private_t *menu_private,
        menu_title_action_t *title_action)
{
    uint8 index;

    if ((title_action->ret_str != NULL) && (title_action->source_id == V_U16_INVALID))
    {
        menu_private->title.data.str = title_action->ret_str;
        menu_private->title.length = -1;
        menu_private->title.language = (int8) sys_comval->language_id;
    }
    else
    {
        if (title_action->title_id != V_U16_INVALID)
        {
            menu_private->title.data.id = title_action->title_id;
            menu_private->title.language = UNICODEID;
        }
        else
        {
            menu_private->title.data.id = g_menu_title.str_id;
            menu_private->title.language = UNICODEID;
        }
    }

    menu_private->item_valid = p_ml_control->bottom - p_ml_control->top + 1;
    menu_private->active = p_ml_control->active;
    menu_private->old = p_ml_control->old;
    menu_private->list_no = p_ml_control->list_no;
    menu_private->list_total = p_ml_control->total;

    for (index = 0; index < menu_private->item_valid; index++)
    {
        if ((title_action->ret_str != NULL) && (title_action->source_id != V_U16_INVALID) && (com_get_confmenu_active(
                &g_menu_title, title_action->source_id) == (p_ml_control->top + index)))
        {
            menu_private->items[index].data.str = title_action->ret_str;
            menu_private->items[index].length = -1;
            menu_private->items[index].language = (int8) sys_comval->language_id;
        }
        else
        {
            menu_private->items[index].data.id = g_menu_items[index].str_id;
            menu_private->items[index].language = UNICODEID;
        }

        if (((g_menu_items[index].child_index & 0x8000) == 0x8000)//RADIO �˵�
                && (p_ml_control->select != V_U16_INVALID)//��������֤��û��ָ�� select ��ʱ��������ʾ RADIO �������û����
                && ((p_ml_control->top + index) == p_ml_control->select))
        {
            menu_private->items[index].argv = ITEMS_BACK_TAG_IS_SELECT;
        }
        else
        {
            menu_private->items[index].argv = ITEMS_BACK_TAG_ISNOT_SELECT;
        }
        //ǰ��ͼ�������˵���󶨵Ĳ˵�ģ��
        if (title_action->style_id == MENULIST_HEAD)
        {
            menu_private->items[index].argv |= ITEMS_FRONT_TAG_IS_DISP;
            menu_private->items[index].argv
                    |= ((uint16) (g_menu_items[index].pic_index) << ITEMS_FRONT_TAG_INDEX_SHIFT);
            menu_private->head_act_id = title_action->head_act_id;
            menu_private->head_id = title_action->head_id;
        }
    }
}

/*! \endcond */
