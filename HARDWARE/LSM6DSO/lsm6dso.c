#include "lsm6dso.h" 
#include "delay.h"
#include "delay.h"

u8 LSM6DSO_Check(void)
{
	u8 temp;
	
	temp=I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_WHO_AM_I);
		   
	if(temp==0X6C)
		return 0;	
	else
	  return 1;	
}
