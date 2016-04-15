/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-19 9:23     1.0             build this file
 *******************************************************************************/
/*!
 * \file     _ui_animation.c
 * \brief    gui ����������֡ͼƬ����ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               ʵ�ֶ������ţ�����֡ͼƬ�������Ź��ܡ�
 * \par      EXTERNALIZED FUNCTIONS:
 *               kernel��libc��ui ������applib���ȵȡ�
 * \version  1.0
 * \date     2011-9-19
 *******************************************************************************/

#include "common_ui.h"

static void __anm_handle(void);

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    �������ţ�����֡ͼƬ�������š�
 * \param[in]    style_infor ui editor������ƵĶ�֡ͼƬ�ؼ���Ϣ�ṹ��ָ��
 * \param[in]    anm_com ָ�򶯻���ʾ��ʼ���ṹ����������ϲ�Զ�����ʾ�Ŀ�����������
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_NULL ������ʾ���������󷵻�AP��
 * \retval           RESULT_REDRAW Ӧ����Ϣ������ RESULT_REDRAW��������other2 �С�
 * \retval           other1 ������ʾ��ֹ���أ����û�gui�������ֹ������key_map_listӳ�䣩��
 * \retval           other2 �յ�����ֵ��Ϊ RESULT_NULL Ӧ����Ϣ���أ��� app_result_e ���塣
 * \ingroup      controls
 * \par          exmaple code
 * \code
 animation_com_data_t anm_com;
 style_infor_t style_infor;
 app_result_e result;

 style_infor.style_id = MUSIC_IN_ANIMATION;
 style_infor.type = UI_AP;
 anm_com.interval = 250;//250ms���
 anm_com.direction = DIRECTION_NORMAL;
 anm_com.forbid = TRUE;//������ֹ������ʾ
 anm_com.key_map_list  = mainmenu_key_map_list;
 anm_com.callback = callback;
 result = gui_animation(&style_infor, &anm_com);
 if((result == RESULT_NULL) || (result == RESULT_REDRAW))
 {
 //�ػ�UI
 }
 else//�������ؽ������Ҫ�����⴦��
 {

 }
 * \endcode
 * \note
 * \li  ������ʾ�����п���ͨ������������ֹ����ʱ forbid ��Ϊ TRUE�������ṩһ��
 *      ������ֹ������ʾ�İ���ӳ���
 *******************************************************************************/
app_result_e gui_animation(style_infor_t *style_infor, animation_com_data_t *anm_com)
{
    input_gui_msg_t input_msg; //��������Ϣ����gui��Ϣ
    msg_apps_type_e gui_event; //gui �¼�
    private_msg_t private_msg; //Ӧ����Ϣ
    app_result_e result = RESULT_NULL;
    int8 timer_id;
    uint8 frame_cnt;
    uint8 tag_backup;

    tag_backup = get_app_timer_tag();
    change_app_timer_tag(APP_TIEMR_TAG_ANIM);

    g_anm_callback = anm_com->callback;
    g_anm_sty_infor = style_infor;
    //������ʱ��
    timer_id = set_app_timer(APP_TIMER_ATTRB_UI, anm_com->interval, __anm_handle);
    if (timer_id == -1)
    {
        change_app_timer_tag(tag_backup);
        return RESULT_NULL;
    }

    //��ȡpicbox֡��
    ui_get_picbox_attrb(g_anm_sty_infor, &frame_cnt, 1);

    g_anm_direction = anm_com->direction;
    if (g_anm_direction == DIRECTION_NORMAL)
    {
        g_anm_index = 0;
    }
    else
    {
        g_anm_index = frame_cnt - 1;
    }

    //���붯����Ϣѭ��
    while (1)
    {
        //�����Ѿ���ʾ���
        if (g_anm_direction == DIRECTION_NORMAL)
        {
            if (g_anm_index >= frame_cnt)
            {
                result = RESULT_NULL;
                break;
            }
        }
        else
        {
            if ((int8) g_anm_index < 0)
            {
                result = RESULT_NULL;
                break;
            }
        }

        //������ֹ������ʾ������Ҫ��ȡ gui ��Ϣ
        if ((anm_com->forbid == TRUE) && (get_gui_msg(&input_msg) == TRUE))//��gui��Ϣ
        {
            //���а���ӳ��
            if (com_key_mapping(&input_msg, &gui_event, anm_com->key_map_list) == TRUE)
            {
                result = gui_event;
                break;
            }
        }
        else
        {
            //���ˣ�gui��Ϣ�����Ѿ��������
            //����ap˽����Ϣ��ϵͳ��Ϣ
            if (get_app_msg(&private_msg) == TRUE)
            {
                result = g_this_app_msg_dispatch(&private_msg);
                if (result != RESULT_NULL)
                {
                    break;
                }
            }
        }

        //����10ms�����������
        sys_os_time_dly(1);
    }

    kill_app_timer(timer_id);
    change_app_timer_tag(tag_backup);
    return result;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ����logo �� �ػ�logo����������Ϣ��
 * \param[in]    style_infor ui editor������ƵĶ�֡ͼƬ�ؼ���Ϣ�ṹ��ָ��
 * \param[in]    anm_com ָ�򶯻���ʾ��ʼ���ṹ����������ϲ�Զ�����ʾ�Ŀ�����������
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_NULL ������ʾ���������󷵻�AP��
 * \ingroup      controls
 * \par          exmaple code
 * \code
 animation_com_data_t anm_com;
 style_infor_t style_infor;

 style_infor.style_id = POWERON_LOGO;
 style_infor.type = UI_AP;
 anm_com.interval = 20;//20ms���
 anm_com.direction = DIRECTION_NORMAL;
 gui_logo(&style_infor, &anm_com);
 * \endcode
 * \note
 * \li  �����ڿ��� logo �� �ػ� logo���ڼ䲻֧���κ���������
 *******************************************************************************/
app_result_e gui_logo(style_infor_t *style_infor, animation_com_data_t *anm_com)
{
    picbox_private_t animation_pic;
    app_result_e result = RESULT_NULL;
    uint16 time_index;
    uint8 frame_cnt, frame_index;

    //��ȡpicbox֡��
    ui_get_picbox_attrb(style_infor, &frame_cnt, 1);

    if (anm_com->direction == DIRECTION_NORMAL)
    {
        frame_index = 0;
    }
    else
    {
        frame_index = frame_cnt - 1;
    }

    //���붯����Ϣѭ��
    while (1)
    {
        //�����Ѿ���ʾ���
        if (anm_com->direction == DIRECTION_NORMAL)
        {
            if (frame_index >= frame_cnt)
            {
                result = RESULT_NULL;
                break;
            }
        }
        else
        {
            if ((int8) frame_index < 0)
            {
                result = RESULT_NULL;
                break;
            }
        }

        //��ʾ����֡
        animation_pic.pic_id = V_U16_INVALID;
        animation_pic.frame_id = frame_index;

        ui_show_picbox(style_infor, &animation_pic);
        if (g_anm_direction == DIRECTION_NORMAL)
        {
            frame_index++;
        }
        else
        {
            frame_index--;//0�ټ�Ϊ0xff��(int8)0xff < 0
        }

        //���𣬵ȴ� interval ʱ��
        for (time_index = 0; time_index < (anm_com->interval / 10); time_index++)
        {
            sys_os_time_dly(1);
        }
    }

    return result;
}

/*! \endcond */

/*! \cond COMMON_API */

static void __anm_handle(void)
{
    picbox_private_t animation_pic;

    //��ʾ��ʱ����
    animation_pic.pic_id = V_U16_INVALID;
    animation_pic.frame_id = g_anm_index;

    ui_show_picbox(g_anm_sty_infor, &animation_pic);
    if (g_anm_direction == DIRECTION_NORMAL)
    {
        g_anm_index++;
    }
    else
    {
        g_anm_index--;//0�ټ�Ϊ0xff��(int8)0xff < 0
    }

    //ÿ֡������ִ�лص�����
    if (g_anm_callback != NULL)
    {
        g_anm_callback();
    }
}

/*! \endcond */
