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
 * \file     _ui_directory_sub.c
 * \brief    commonUI �ļ�������ؼ�������ʵ��
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����һ���ļ������GUI�������ļ�ϵͳ�������б��ղؼе��ļ������
 * \par      EXTERNALIZED FUNCTIONS:
 *               ֱ�ӵ���ui �����е�listbox��ɿؼ���ʾ������
 * \version 1.0
 * \date  2011-9-18
 *******************************************************************************/

#include "common_ui.h"

///�ļ�����ٶȿ�������
const uint16 line_cnt_vec[] =
{ 8, 16 };
void move_file_records(file_brow_t *file_list, dir_control_t *p_dir_control, uint16 lines);
bool init_dir_control(dir_control_t *p_dir_control);

//�����·�����
void get_line_cnt(void)
{
    uint8 list_no_max_index = sizeof(line_cnt_vec) / sizeof(uint16);
    uint8 i;

    if (speed_control <= line_cnt_vec[list_no_max_index - 1])
    {
        speed_control++;
    }

    for (i = 0; i < list_no_max_index; i++)
    {
        if (speed_control <= line_cnt_vec[i])
        {
            line_cnt = i + 1;
            return;
        }
    }

    line_cnt = list_no_max_index;
    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ѡ���ļ���Ŀ¼���б��е��¼���
 * \param[in]    p_dir_control��ָ���ļ���Ŀ¼���б���ƽṹ��
 * \param[in]    file_list���ļ�����б��������к�������ʱ������Ч���ݰ���
 * \param[out]   p_dir_control�����ظ����˵��ļ���Ŀ¼���б����
 * \return       none
 * \note
 *******************************************************************************/
void dir_select_next(file_brow_t *file_list, dir_control_t *p_dir_control)
{
    get_line_cnt();

    //��ҳ���л����к���δ��ҳ�����һ���ൽ�����ڶ�������¼�����
    if ((p_dir_control->list_no + line_cnt) < p_dir_control->bottom)
    {
        p_dir_control->list_no += line_cnt;
        p_dir_control->old = p_dir_control->active;
        p_dir_control->active += line_cnt;
        p_dir_control->draw_mode = LIST_DRAW_ACTIVE;
    }
    else
    {
        //���б����һ������б�ͷ��ʾ
        if (p_dir_control->list_no == p_dir_control->list_total)
        {
            p_dir_control->list_no = 1;
            //����һҳ������£�ֻ����¼�����
            if (p_dir_control->list_total <= p_dir_control->one_page_count)
            {
                //����������1�����Ҫ�л�
                if (p_dir_control->list_total > 1)
                {
                    p_dir_control->old = p_dir_control->active;
                    p_dir_control->active = 1;
                    p_dir_control->draw_mode = LIST_DRAW_ACTIVE;
                }
            }
            //��β�л���ͷ�����������б�
            else
            {
                p_dir_control->top = 1;
                p_dir_control->bottom = p_dir_control->one_page_count;
                p_dir_control->old = p_dir_control->active = 1;
                p_dir_control->update_mode = LIST_UPDATE_HEAD;
                p_dir_control->draw_mode = LIST_DRAW_LIST;
            }
        }
        else
        {
            //���б�β�����е����һ��
            if (p_dir_control->list_no >= (p_dir_control->list_total - line_cnt))
            {
                //�պ��ڵ�ǰҳ�е����һ�ֻ����¼�����
                if ((p_dir_control->list_no == (p_dir_control->list_total - line_cnt)) && (p_dir_control->bottom
                        == p_dir_control->list_total))
                {
                    p_dir_control->list_no += line_cnt;
                    p_dir_control->old = p_dir_control->active;
                    p_dir_control->active += line_cnt;
                    p_dir_control->draw_mode = LIST_DRAW_ACTIVE;
                }
                //���򣬸��������б�
                else
                {
                    //����ʵ���л�����
                    line_cnt = p_dir_control->list_total - p_dir_control->bottom;
                    //����p_dir_control�ṹ
                    p_dir_control->bottom = p_dir_control->list_no = p_dir_control->list_total;
                    if (p_dir_control->bottom >= p_dir_control->one_page_count)
                    {
                        p_dir_control->top = p_dir_control->bottom - p_dir_control->one_page_count + 1;
                    }
                    else
                    {
                        p_dir_control->top = 1;
                    }
                    p_dir_control->old = p_dir_control->active = p_dir_control->bottom - p_dir_control->top + 1;
                    p_dir_control->update_mode = LIST_UPDATE_NEXT;
                    p_dir_control->draw_mode = LIST_DRAW_LIST;
                }
            }
            //�ڵ�ǰҳβ�������Һ������㹻�б��������ҳ�����������б�
            else
            {
                //��ǰҳ�����ڶ���֮ǰ
                if (p_dir_control->active < (p_dir_control->one_page_count - 1))
                {
                    p_dir_control->list_no += line_cnt;
                    p_dir_control->bottom = p_dir_control->list_no + 1;
                    p_dir_control->top = p_dir_control->bottom - p_dir_control->one_page_count + 1;
                    p_dir_control->old = p_dir_control->active = p_dir_control->one_page_count - 1;
                }
                //��ǰҳ�����ڶ���
                else
                {
                    p_dir_control->list_no += line_cnt;
                    p_dir_control->top += line_cnt;
                    p_dir_control->bottom += line_cnt;
                    p_dir_control->old = p_dir_control->active;
                }

                p_dir_control->update_mode = LIST_UPDATE_NEXT;
                p_dir_control->draw_mode = LIST_DRAW_LIST;
            }
        }
    }

    //�ƶ���Ч�б���
    if (p_dir_control->update_mode == LIST_UPDATE_NEXT)
    {
        move_file_records(file_list, p_dir_control, line_cnt);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ѡ���ļ���Ŀ¼���б��е��ϼ���
 * \param[in]    p_dir_control��ָ���ļ���Ŀ¼���б���ƽṹ��
 * \param[in]    file_list���ļ�����б��������к�������ʱ������Ч���ݰ���
 * \param[out]   p_dir_control�����ظ����˵��ļ���Ŀ¼���б����
 * \return       none
 * \note
 *******************************************************************************/
void dir_select_prev(file_brow_t *file_list, dir_control_t *p_dir_control)
{
    get_line_cnt();

    //��ҳ���л����к���δ��ҳ�������ൽ�ڶ�������¼�����
    if (p_dir_control->active > (line_cnt + 1))
    {
        p_dir_control->list_no -= line_cnt;
        p_dir_control->old = p_dir_control->active;
        p_dir_control->active -= line_cnt;
        p_dir_control->draw_mode = LIST_DRAW_ACTIVE;
    }
    else
    {
        //���б��һ������б�β��ʾ
        if (p_dir_control->list_no == 1)
        {
            p_dir_control->list_no = p_dir_control->list_total;
            //����һҳ������£�ֻ����¼�����
            if (p_dir_control->list_total <= p_dir_control->one_page_count)
            {
                //����������1�����Ҫ�л�
                if (p_dir_control->list_total > 1)
                {
                    p_dir_control->old = p_dir_control->active;
                    p_dir_control->active = p_dir_control->list_total;
                    p_dir_control->draw_mode = LIST_DRAW_ACTIVE;
                }
            }
            //��ͷ�е�β�����������б�
            else
            {
                p_dir_control->top = p_dir_control->list_total - p_dir_control->one_page_count + 1;
                p_dir_control->bottom = p_dir_control->list_total;
                p_dir_control->old = p_dir_control->active = p_dir_control->one_page_count;
                p_dir_control->update_mode = LIST_UPDATE_TAIL;
                p_dir_control->draw_mode = LIST_DRAW_LIST;
            }
        }
        else
        {
            //���б�ͷ�����е���һ��
            if (p_dir_control->list_no <= (1 + line_cnt))
            {
                //�պ��ڵ�ǰҳ�е���һ�ֻ����¼�����
                if ((p_dir_control->list_no == (1 + line_cnt)) && (p_dir_control->top == 1))
                {
                    p_dir_control->list_no -= line_cnt;
                    p_dir_control->old = p_dir_control->active;
                    p_dir_control->active -= line_cnt;
                    p_dir_control->draw_mode = LIST_DRAW_ACTIVE;
                }
                //���򣬸��������б�
                else
                {
                    //����ʵ���л�����
                    line_cnt = p_dir_control->top - 1;
                    //����p_dir_control�ṹ
                    p_dir_control->top = p_dir_control->list_no = 1;
                    init_dir_control(p_dir_control);
                    p_dir_control->update_mode = LIST_UPDATE_PREV;
                    p_dir_control->draw_mode = LIST_DRAW_LIST;
                }
            }
            //�ڵڶ������ǰ�����㹻�б��������ҳ�����������б�
            else
            {
                //��ǰҳ�ڶ���֮��
                if (p_dir_control->active > 2)
                {
                    p_dir_control->list_no -= line_cnt;
                    p_dir_control->top = p_dir_control->list_no - 1;
                    p_dir_control->bottom = p_dir_control->top + p_dir_control->one_page_count - 1;
                    p_dir_control->old = p_dir_control->active = 2;
                }
                //��ǰҳ�ڶ���
                else
                {
                    p_dir_control->list_no -= line_cnt;
                    p_dir_control->top -= line_cnt;
                    p_dir_control->bottom -= line_cnt;
                    p_dir_control->old = p_dir_control->active;
                }

                p_dir_control->update_mode = LIST_UPDATE_PREV;
                p_dir_control->draw_mode = LIST_DRAW_LIST;
            }
        }
    }

    //�ƶ���Ч�б���
    if (p_dir_control->update_mode == LIST_UPDATE_PREV)
    {
        move_file_records(file_list, p_dir_control, line_cnt);
    }
}

//��ʼ��dir_control_t���ݽṹ
bool init_dir_control(dir_control_t *p_dir_control)
{
    p_dir_control->bottom = p_dir_control->top + p_dir_control->one_page_count - 1;
    if (p_dir_control->bottom > p_dir_control->list_total)
    {
        p_dir_control->bottom = p_dir_control->list_total;
    }
    p_dir_control->old = p_dir_control->active = p_dir_control->list_no - p_dir_control->top + 1;

    return TRUE;
}

//�ƶ���Ч�ļ���¼����� LIST_UPDATE_NEXT �� LIST_UPDATE_PREV ��Ч
void move_file_records(file_brow_t *file_list, dir_control_t *p_dir_control, uint16 lines)
{
    file_brow_t *from, *to;
    uint16 begin, end, i;

    switch (p_dir_control->update_mode)
    {
        case LIST_UPDATE_NEXT:
        begin = lines;
        end = p_dir_control->one_page_count;
        for (i = begin; i < end; i++)
        {
            from = file_list + i;
            to = file_list + i - lines;
            libc_memcpy(to->ext, from->ext, sizeof(to->ext));
            libc_memcpy(to->name_buf, from->name_buf, (uint32) (to->name_len));
        }
        file_list += p_dir_control->one_page_count - 1;
        break;
        case LIST_UPDATE_PREV:
        begin = 0;
        end = p_dir_control->one_page_count - lines;
        for (i = end; i > begin; i--)
        {
            from = file_list + i - 1;
            to = file_list + i + lines - 1;
            libc_memcpy(to->ext, from->ext, sizeof(to->ext));
            libc_memcpy(to->name_buf, from->name_buf, (uint32) (to->name_len));
        }
        break;
        default:
        break;
    }
}

