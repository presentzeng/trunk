#include "Dac_drv.h"
#include "TWI.h"
#include "case_type.h"
#include "Dac_4398.h"

int DacDrv_Init(void* null1, void* null2, void* null3)
{
    return 0;
}

int DacDrv_Exit(void* null1, void* null2, void* null3)
{
    return 0;
}

module_init(DacDrv_Init)
module_exit(DacDrv_Exit) 
