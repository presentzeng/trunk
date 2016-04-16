#include "common_ui.h"
#define DAC_VOLUME_MAX 255 //
#define DAC_VOLUME_MID 30 //

uint8 dac_volume_caculate(uint8 max, uint8 current)
{
    uint8 flag = -1;
    if(current  > max / 2)
    {
        flag = (max - current) * 2;
    }
    else
    {
        flag =  DAC_VOLUME_MAX - (DAC_VOLUME_MAX / (max / 2)) * current;
    }
	return flag;
}