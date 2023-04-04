#include "ap3216c.h" 
#include "delay.h"
#include "myiic.h"

u8 AP3216C_Check_And_Init(void)
{
	u8 temp;
	
	I2C_WriteOneByte(AP3216C_ADDR,AP3216C_SYSTEMCONG,AP3216C_SW_RESET);   /*   SW reset   */
	delay_ms(50);
	I2C_WriteOneByte(AP3216C_ADDR,AP3216C_SYSTEMCONG,AP3216C_ENABLE_ALL);   /* enable ALS PS IR */
	
	temp=I2C_ReadOneByte(AP3216C_ADDR,AP3216C_SYSTEMCONG);
		   
	if(temp == AP3216C_ENABLE_ALL)
		return 0;	
	else
	  return 1;	
}


