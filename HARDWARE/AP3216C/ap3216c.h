#include "myiic.h"
#include "sys.h"


#define AP3216C_ADDR    	(0X1E << 1)


#define AP3216C_SYSTEMCONG		0x00
#define AP3216C_INTSTATUS			0X01
#define AP3216C_INTCLEAR			0X02

#define AP3216C_IRDATALOW			0x0A
#define AP3216C_IRDATAHIGH		0x0B
#define AP3216C_ALSDATALOW		0x0C
#define AP3216C_ALSDATAHIGH		0X0D
#define AP3216C_PSDATALOW			0X0E
#define AP3216C_PSDATAHIGH		0X0F

/*

system config register 

000: Power down (Default)
001: ALS function active
010: PS+IR function active
011: ALS and PS+IR functions active
100: SW reset
101: ALS function once
110: PS+IR function once
111: ALS and PS+IR functions once
*/
#define AP3216C_POWER_DOWN			0x00
#define AP3216C_ENABLE_ALL			0x03
#define AP3216C_SW_RESET			  0x04


u8 AP3216C_Check_And_Init(void);









