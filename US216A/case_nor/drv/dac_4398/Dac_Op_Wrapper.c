#include "Dac_drv.h"
#include "Dac_4398.h"

//要加本地信息



//int sDAC_Init(uint8 band, uint8 level, uint32 freq)
int sDAC_Init(void * a, void * b, void * c)
{

*((REG32) (GPIOAINEN)) &= (~0x00000008);
*((REG32) (GPIOAOUTEN)) |= (0x00000008);
*((REG32) (GPIOADAT)) &= (~0x00000008);
sys_os_time_dly(10);
*((REG32) (GPIOADAT))  |= (0x00000008);
DAC_WriteReg(0x08,0xc0); 
DAC_WriteReg(0x02,0x10); 
DAC_WriteReg(0x03,0x89);              
DAC_WriteReg(0x04,0x82); 
DAC_WriteReg(0x08,0x40); 
DAC_WriteReg(0x05,0x1a); 
DAC_WriteReg(0x06,0x1a);

    return 1;
}

int sDAC_Set_Volume(UINT32 vol, void *b, void *c)
{
	DAC_WriteReg(0x05,  (UINT8) vol); 
	DAC_WriteReg(0x06,  (UINT8) vol); 
	DAC_ReadReg(0x05);
}

