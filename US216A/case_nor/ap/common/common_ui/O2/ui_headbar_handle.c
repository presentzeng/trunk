/*******************************************************************************
 *                              us212A
 *                            Module: Common
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved. 
 *
 * History:         
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2012-5-17 11:13     1.0             build this file 
*******************************************************************************/
/*!
 * \file     _ui_headbar_handle.c
 * \brief    ״̬��״̬���ʵ�֡�
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               
 * \par      EXTERNALIZED FUNCTIONS:
 *               
 * \version  1.0
 * \date     2012-5-17
*******************************************************************************/

#include "common_ui.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!                    
 * \par  Description:
 *    ״̬��״̬��⣬����и��£����� gui_headbar ���и��¡�
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      controls
 * \note
 * \li  �ýӿڿ��Կ����� gui_headbar �ļ򻯽ӿڣ�ֻʵ�����ⲿ�ֹ��ܡ�
*******************************************************************************/
void gui_headbar_handle(void)
{
    time_t temp_sys_time;
    uint8 temp_cable_state;
    uint8 temp_card_state;
    uint8 temp_icon_id;
    bool need_update_headbar = FALSE;
    
    do//ִ��1��ѭ����Ϊ��ǰ����һ���ط�������������break�˳�ѭ��
    {
        if(this_headbar_style.battery_sty != V_U16_INVALID)
        {
            if((g_app_info_state.charge_state == BAT_CHARGING)//���ڳ��
            || (g_app_info_state.charge_state == BAT_FULL)//�������
            || (g_app_info_state.bat_value != g_battery_value))//�޳�磬��ع��磬�ҵ����仯
            {
                need_update_headbar = TRUE;
                break;
            }
        }
        
        if(this_headbar_style.icon_sty != V_U16_INVALID)
        {
            if(g_app_info_state.keylock_state == TRUE)
            {
                temp_icon_id = HEADBAR_ICON_KEYLOCK;
            }
            else
            {
                temp_icon_id = this_headbar_icon_id;
            }
            
            if((temp_icon_id != HEADBAR_ICON_NONE) && (temp_icon_id != g_icon_id))
            {
                need_update_headbar = TRUE;
                break;
            }
        }
    
        if(this_headbar_style.time_sty != V_U16_INVALID)
        {
            //��ȡ��ǰʱ��
            sys_get_time(&temp_sys_time);
            
            if(temp_sys_time.minute != g_sys_minute)
            {
                need_update_headbar = TRUE;
                break;
            }
        }
    
        if(this_headbar_style.cable_sty != V_U16_INVALID)
        {
            temp_cable_state = get_cable_state();
            
            if(temp_cable_state != g_app_last_state.last_cable_state)
            {
                need_update_headbar = TRUE;
                break;
            }
        }
    
        if(this_headbar_style.card_sty != V_U16_INVALID)
        {
            temp_card_state = get_card_state();
            
            if(temp_card_state != g_app_last_state.last_card_state)
            {
                need_update_headbar = TRUE;
                break;
            }
        }
    }while(0);
    
    //��Ҫ���� headbar
    if(need_update_headbar == TRUE)
    {
        gui_headbar(HEADBAR_UPDATE_CHANGE);
    }
}

/*! \endcond */
