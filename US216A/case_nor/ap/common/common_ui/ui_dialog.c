/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-14 9:21     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_dialog.c
 * \brief    gui ���������Ի���ؼ�������ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ����֪ͨ��ѯ����;�ĶԻ���ؼ���
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��ui ������applib���ȵȡ�
 * \version  1.0
 * \date     2011-9-14
 *******************************************************************************/

#include "common_ui.h"

//��ͬ���Ͱ����б�ÿ��������Ӧһ�������Ϣ
const button_list_item_t button_list[] =
{
{ 2,
{
{ S_BUTTON_OK, RESULT_DIALOG_OK },
{ S_BUTTON_CANCEL, RESULT_REDRAW },
{ 0, 0 } } },
{ 3,
{
{ S_BUTTON_ABORT, RESULT_DIALOG_ABORT },
{ S_BUTTON_RETRY, RESULT_DIALOG_RETRY },
{ S_BUTTON_IGNORE, RESULT_REDRAW } } },
//{3, {{S_BUTTON_YES, RESULT_DIALOG_YES}, {S_BUTTON_NO, RESULT_DIALOG_NO}, {S_BUTTON_CANCEL, RESULT_REDRAW}}},
        { 2,
        {
        { S_BUTTON_YES, RESULT_DIALOG_YES },
        { S_BUTTON_NO, RESULT_DIALOG_NO },
        { 0, 0 } } },
        { 2,
        {
        { S_BUTTON_YES, RESULT_DIALOG_YES },
        { S_BUTTON_NO, RESULT_REDRAW },
        { 0, 0 } } },
        { 2,
        {
        { S_BUTTON_RETRY, RESULT_DIALOG_RETRY },
        { S_BUTTON_CANCEL, RESULT_REDRAW },
        { 0, 0 } } },
        { 2,
        {
        { 0, 0 },
        { 0, 0 },
        { 0, 0 } } } };

//dialog gui app msg deal
app_result_e dialog_msg_deal(dialog_com_data_t *dialog_com, dialog_private_t *dialog_private, bool any_key_flag)
{
    input_gui_msg_t input_msg;
    msg_apps_type_e gui_event;
    private_msg_t private_msg;
    app_result_e result = RESULT_NULL;

    //��ȡ�� gui��Ϣ
    if (get_gui_msg(&input_msg) == TRUE)
    {
        //�����ⰴ���˳��ĶԻ���ģʽ
        if (any_key_flag == TRUE)
        {
            if ((INPUT_MSG_KEY == input_msg.type) || (INPUT_MSG_TOUCH == input_msg.type))
            {
                result = RESULT_REDRAW;
                //�������˺�������
                com_filter_key_hold();
            }
        }

        //���� gui��Ϣӳ��
        if (com_key_mapping(&input_msg, &gui_event, dialog_key_map_list) == TRUE)
        {
            switch (gui_event)
            {
                case EVENT_DIALOG_NEXT_BUTTON:
                //ѡ����һ����ťѡ��
                dialog_private->old = dialog_private->active;
                if (dialog_private->active < (dialog_private->button_cnt - 1))
                {
                    dialog_private->active++;
                }
                else
                {
                    dialog_private->active = 0;
                }
                dialog_com->draw_mode = DIALOG_DRAW_BUTTON;
                break;

                case EVENT_DIALOG_PREV_BUTTON:
                //ѡ����һ����ťѡ��
                dialog_private->old = dialog_private->active;
                if (dialog_private->active > 0)
                {
                    dialog_private->active--;
                }
                else
                {
                    dialog_private->active = dialog_private->button_cnt - 1;
                }
                dialog_com->draw_mode = DIALOG_DRAW_BUTTON;
                break;

                case EVENT_DIALOG_CONFIRM:
                //ȷ��ѡ���
                result = button_list[(uint8) dialog_com->button_type].buttons[dialog_private->active].result;
                break;

                case EVENT_DIALOG_CANCEL:
                //ȡ������
                result = RESULT_REDRAW;
                break;

                default:
                result = com_message_box(gui_event);
                if (result == RESULT_REDRAW)
                {
                    dialog_com->draw_mode = DIALOG_DRAW_ALL;
                    result = RESULT_NULL;//���˳��ؼ�
                }
                break;
            }
        }
    }
    //���ˣ�gui��Ϣ�����Ѿ�������ϣ���ȡap˽����Ϣ��ϵͳ��Ϣ������ת��Ϊ˽����Ϣ�ٷ��أ�������
    else
    {
        if (get_app_msg(&private_msg) == TRUE)
        {
            result = g_this_app_msg_dispatch(&private_msg);
            if (result == RESULT_REDRAW)
            {
                dialog_com->draw_mode = DIALOG_DRAW_ALL;
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
 *    �Ի���ؼ�������ʵ�֡�
 * \param[in]    style_id ui editor������ƵĶԻ���ؼ�ģ��
 * \param[in]    dialog_com ָ��Ի���ؼ���ʼ���ṹ����������ϲ�ԶԻ���Ŀ�����������
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_DIALOG_XXX ѡ������Ч�����󷵻أ���ϸ�μ� app_result_e ����
 * \retval           RESULT_REDRAW ��Ч���أ�AP��Ҫ�ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \ingroup      controls
 * \par          exmaple code
 * \code
 dialog_com_data_t dialog_com;
 app_result_e result;

 dialog_com.dialog_type = DIALOG_BUTTON_INCLUDE;
 dialog_com.button_type = BUTTON_YESNO;
 dialog_com.icon_id = V_U16_INVALID;
 dialog_com.string_desc = &string_desc;
 dialog_com.active_default = 0;
 result = gui_dialog(DIALOG_ASK, &dialog_com);
 if(result == DIALOG_RESULT_YES)//ѡ�� Yes ��ť��ִ�в���
 {

 }
 else if(result == RESULT_REDRAW)//ȡ����ѡ�� No ��ť�������ػ�
 {

 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 *******************************************************************************/
app_result_e gui_dialog(uint16 style_id, dialog_com_data_t *dialog_com)
{
    dialog_private_t dialog_private;
    style_infor_t style_infor;
    app_result_e result = RESULT_NULL;
    bool any_key_flag = FALSE;
    uint8 tag_backup;
    uint8 mode_backup, icon_backup;

    //draw bg
    ui_set_backgd_color(Color_BACKCOLOR_def);
    ui_clear_screen(NULL); //2015-10-30

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_DIALOG);

    gui_get_headbar_mode(&mode_backup, &icon_backup);
    gui_set_headbar_mode(HEADBAR_MODE_NULL, this_headbar_icon_id);

    dialog_private.icon_id = dialog_com->icon_id;
    dialog_private.string_desc = dialog_com->string_desc;

    style_infor.style_id = style_id;
    style_infor.type = UI_COM;

    dialog_com->draw_mode = DIALOG_DRAW_ALL;

    //û�а����ĶԻ�����
    if (dialog_com->dialog_type != DIALOG_BUTTON_INCLUDE)
    {
        //����dialogbox
        dialog_private.button_cnt = 0;
        ui_show_dialogbox(&style_infor, &dialog_private, (uint32) dialog_com->draw_mode);
        dialog_com->draw_mode = DIALOG_DRAW_NULL;
#ifdef PC
        UpdateMainWnd();
#endif
        //���������Ϣ��ʾ�������˳�
        if (dialog_com->dialog_type == DIALOG_INFOR_IMMEDIATE)
        {
            change_app_timer_tag(tag_backup);
            gui_set_headbar_mode(mode_backup, icon_backup);
            return RESULT_REDRAW;
        }
        //�����ⰴ���˳�
        else if (dialog_com->dialog_type == DIALOG_BUTTON_ANYKEY)
        {
            any_key_flag = TRUE;
        }
        //�������Ҫ�ȴ�����Ϣ��ʾ�����ȵȴ�2���Ӻ��˳�
        else
        {
            com_app_sleep(4, NULL);
            change_app_timer_tag(tag_backup);
            gui_set_headbar_mode(mode_backup, icon_backup);
            return RESULT_REDRAW;
        }
    }
    //�а����ĶԻ������ȳ�ʼ��
    else
    {
        uint8 i;

        dialog_private.button_cnt = button_list[(uint8) dialog_com->button_type].button_cnt;
        for (i = 0; i < dialog_private.button_cnt; i++)
        {
            dialog_private.buttons[i] = button_list[(uint8) dialog_com->button_type].buttons[i].str_id;
        }

        dialog_private.old = dialog_private.active = dialog_com->active_default;
    }

    //����Ի�����Ϣѭ��
    while (1)
    {
        //ˢ��dialogbox
        if (dialog_com->draw_mode != DIALOG_DRAW_NULL)
        {
            ui_show_dialogbox(&style_infor, &dialog_private, (uint32) dialog_com->draw_mode);
            dialog_com->draw_mode = DIALOG_DRAW_NULL;
        }

        result = dialog_msg_deal(dialog_com, &dialog_private, any_key_flag);
        if (result != RESULT_NULL)
        {
            break;
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    change_app_timer_tag(tag_backup);
    gui_set_headbar_mode(mode_backup, icon_backup);
    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ��Ϣ��ʾ�ͶԻ��������ʵ�֡�
 * \param[in]    style_id ui editor������ƵĶԻ���ؼ�ģ��
 * \param[in]    type �Ի������ͣ�ֻ���� DIALOG_BUTTON_ANYKEY��DIALOG_INFOR_WAIT��
 *                     DIALOG_INFOR_IMMEDIATE �е�һ��
 * \param[in]    str_id ��������ַ���ID
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_DIALOG_XXX ѡ������Ч�����󷵻أ���ϸ�μ� app_result_e ����
 * \retval           RESULT_REDRAW ��Ч���أ�AP��Ҫ�ػ�UI
 * \retval           other �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ����
 * \ingroup      controls
 * \par          exmaple code
 * \code
 app_result_e result;

 result = gui_dialog_msg(DIALOG_MSG, DIALOG_INFOR_IMMEDIATE, S_WAITING);//��ʾ�����Ժ󡭡�
 if(result == RESULT_REDRAW)//��ʾ�����������أ��ػ�UI
 {

 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 * \li  �Ի�������ֻ���� DIALOG_BUTTON_ANYKEY��DIALOG_INFOR_WAIT��DIALOG_INFOR_IMMEDIATE
 *      �е�һ�֣�����ǿ��Ϊ DIALOG_INFOR_WAIT��
 *******************************************************************************/
app_result_e gui_dialog_msg(uint16 style_id, dialog_type_e type, uint16 str_id)
{
    dialog_com_data_t dialog_param;
    string_desc_t str_desc;

    str_desc.data.id = str_id;
    str_desc.language = UNICODEID;

    //�Ի������� ��ʾ
    if (type == DIALOG_BUTTON_INCLUDE)
    {
        type = DIALOG_INFOR_WAIT;
    }
    dialog_param.dialog_type = type;
    dialog_param.icon_id = -1;
    //�ַ���
    dialog_param.string_desc = &str_desc;

    //��������
    dialog_param.button_type = BUTTON_NO_BUTTON;

    //���� gui_dialog ��ʾ��Ϣ
    return gui_dialog(style_id, &dialog_param);
}

/*! \endcond */
