#include "hmc5883.h" 
#include "delay.h"
#include "myiic.h"
#include "myiic2.h"

u8 HMC5883_Check(void)
{
	u8 tempA;
	u8 tempB;
	u8 tempC;
	
	tempA=I2C2_ReadOneByte(HMC5883_ADDR,HMC_CHEAK_A_REG);
	tempB=I2C2_ReadOneByte(HMC5883_ADDR,HMC_CHEAK_B_REG);
	tempC=I2C2_ReadOneByte(HMC5883_ADDR,HMC_CHEAK_C_REG);
		   
	if(tempA == HMC_CHECKA_VALUE & tempB == HMC_CHECKB_VALUE & tempC == HMC_CHECKC_VALUE)
		return 0;	
	else
	  return 1;	
}

void Hmc5883_Init(void)
{
	I2C2_WriteOneByte(HMC5883_ADDR,HMC_CONFIG_A_REG,0x78);  //set 75Hz
	I2C2_WriteOneByte(HMC5883_ADDR,HMC_CONFIG_B_REG,0x00);
	I2C2_WriteOneByte(HMC5883_ADDR,HMC_MODE_REG,0x00);  //cotinue mode
}


