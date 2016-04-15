#include "Dac_4398.h"
#include "Dac_drv.h"

void SwitchGPIO(void)
{
    TWI_SDA_Output_Dis(twi_gpio_dac);
    TWI_SDA_Input_Dis(twi_gpio_dac);
    TWI_SCL_Output_Dis(twi_gpio_dac);
    TWI_SCL_Input_Dis(twi_gpio_dac);
}

UINT8 DAC_WriteReg(UINT8 Regis_Addr, UINT8 Data)
{
    uint8 i;
    int result = 0;
    uint8 writebuffer[2];

    uint32 flags;

    //close irq
    flags = sys_local_irq_save();

    writebuffer[0] = Regis_Addr;
    //register data
    writebuffer[1] = Data;

    for (i = 0; i < 10; i++)
    {
        //send 3 byte to fm module
        if (TWI_Trans_Bytes(writebuffer, DAC_WRITE_ADDR,
                sizeof(writebuffer), twi_gpio_dac, &delay_twi_dac) != 0)
        {
            break;
        }
    }
	SwitchGPIO();
    //open irq
    sys_local_irq_restore(flags);
    if (i == 10)
    {
        result = -1;
    }
    return result;
}


UINT8 DAC_ReadReg(UINT8 Regis_Addr)
{
    uint8 Data = 0;
    int result = 0;

    //close irq
    uint32 flags;
    flags = sys_local_irq_save(); 

    TWI_Init(twi_gpio_dac, &delay_twi_dac);
    TWI_Start(twi_gpio_dac, &delay_twi_dac);

    TWI_WriteByte(DAC_WRITE_ADDR, twi_gpio_dac, &delay_twi_dac);

    if (TWI_GetAck(twi_gpio_dac, &delay_twi_dac) != 0)
    {
        
        TWI_WriteByte(Regis_Addr, twi_gpio_dac, &delay_twi_dac);
            if (TWI_GetAck(twi_gpio_dac, &delay_twi_dac) != 0)
            {
            //TWI_Stop(twi_gpio, &delay_twi_dac);
            TWI_Start(twi_gpio_dac, &delay_twi_dac);
            //send TWI read addr
            TWI_WriteByte(DAC_WRITE_ADDR | 1, twi_gpio_dac, &delay_twi_dac);

            if (TWI_GetAck(twi_gpio_dac, &delay_twi_dac) != 0)
            {
                //receive data
                Data = TWI_ReadByte(twi_gpio_dac, &delay_twi_dac);
                
                TWI_SendAck(1, twi_gpio_dac, &delay_twi_dac);
            }
        }
    }

    TWI_Stop(twi_gpio_dac, &delay_twi_dac);
    SwitchGPIO();
    sys_local_irq_restore(flags);
    return Data;
}