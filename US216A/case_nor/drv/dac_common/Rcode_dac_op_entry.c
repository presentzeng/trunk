#include "Dac_interface.h"
#include "Dac_drv.h"

//��ʱ���� ����Ƶ��� ���ݾ����������
#define PARAM_DELAY_TWI   20

gpio_init_cfg_t gpio_twi_dac[2] =
{
    { GPIOBINEN, GPIOBOUTEN, GPIOBDAT, GPIO_SCL_BIT },
    //{ GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SCL_BIT },
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SDA_BIT }
    //{ GPIOBINEN, GPIOBOUTEN, GPIOBDAT, GPIO_SDA_BIT }
};

/* TWI ����ָ��*/
gpio_init_cfg_t *twi_gpio_dac = (gpio_init_cfg_t *) gpio_twi_dac;

uint8 delay_twi_dac = PARAM_DELAY_TWI;

dac_driver_operations dac_drv_op =
{
    (dac_op_func) sDAC_Init,                               //��ʼ��
    (dac_op_func) sDAC_Set_Volume,              //��������
};

