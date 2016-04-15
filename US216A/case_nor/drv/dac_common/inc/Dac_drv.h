/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
  描述: 外挂Dac驱动

*********************************************************************************/



#ifndef _DAC_DRV_H
#define _DAC_DRV_H


#include "TWI.h"
#include "psp_includes.h"
#include "dac_interface.h"

/* dac驱动公用全局变量 */
extern gpio_init_cfg_t *twi_gpio_dac;
extern uint8 delay_twi_dac;

/* i2c 驱动声明 */
uint8 TWI_Trans_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay);
uint8 TWI_Recev_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay);
void TWI_Start(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_Stop(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_WriteByte(uint8 dat, gpio_init_cfg_t* gpio, uint8* delay);
uint8 TWI_ReadByte(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_Init(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_Exit(gpio_init_cfg_t* gpio);
void TWI_SendAck(uint8 ack, gpio_init_cfg_t* gpio, uint8* delay);
uint8 TWI_GetAck(gpio_init_cfg_t* gpio, uint8* delay);


//int sDAC_Init(uint8 band, uint8 level, uint32 freq) __FAR__;
int sDAC_Init(void *a, void *b, void *c) __FAR__;
int sDAC_Set_Volume(UINT32 vol, void *b, void *c) __FAR__;


#endif 
