#include "Dac_interface.h"
#include "Dac_drv.h"

//延时操作 和主频相关 根据具体情况设置
#define PARAM_DELAY_TWI   20

gpio_init_cfg_t gpio_twi_dac[2] =
{
    { GPIOBINEN, GPIOBOUTEN, GPIOBDAT, GPIO_SCL_BIT },
    //{ GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SCL_BIT },
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SDA_BIT }
    //{ GPIOBINEN, GPIOBOUTEN, GPIOBDAT, GPIO_SDA_BIT }
};

/* TWI 配置指针*/
gpio_init_cfg_t *twi_gpio_dac = (gpio_init_cfg_t *) gpio_twi_dac;

uint8 delay_twi_dac = PARAM_DELAY_TWI;

dac_driver_operations dac_drv_op =
{
    (dac_op_func) sDAC_Init,                               //初始化
    (dac_op_func) sDAC_Set_Volume,              //音量控制
};

