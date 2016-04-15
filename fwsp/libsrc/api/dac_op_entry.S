/*
*********************************************************************************************************
*                                               MIPS Port
*
*                 Target           : MIPS (Includes 4Kc)
*                 Ported by        : 
*                 URL              : 
*
*********************************************************************************************************
*/

#include <asm-mips/regdef.h>
#include <asm-mips/asm.h>
#include <asm-mips/m32c0.h>
#include <asm-mips/addrspace.h>
#include <api.h>

    .set mips16
    .set noat
    .set noreorder
/*
**********************************************************************************************************
*                                          page_miss_exception
*
**********************************************************************************************************
*/


LEAF(dac_op_entry)
    .set noreorder
    .text
    li v1,API_I2C_ENTRY
    break 0
END (dac_op_entry)