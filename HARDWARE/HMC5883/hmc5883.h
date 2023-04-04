#include "myiic.h"
#include "myiic2.h"
#include "sys.h"

#define HMC5883_ADDR    		(0X1e << 1)
 
#define HMC_CONFIG_A_REG    0X00
#define HMC_CONFIG_B_REG    0X01  
#define HMC_MODE_REG        0X02 
  
#define HMC_XMSB_REG        0X03    //X
#define HMC_XLSB_REG        0X04  
  
#define HMC_ZMSB_REG        0X05    //Z 
#define HMC_ZLSB_REG        0X06  
  
#define HMC_YMSB_REG        0X07    //Y  
#define HMC_YLSB_REG        0X08  
  
#define HMC_STATUS_REG      0X09   
  
#define HMC_CHEAK_A_REG     0X0A 
#define HMC_CHEAK_B_REG     0X0B  
#define HMC_CHEAK_C_REG     0X0C  

//value
  
#define HMC_CHECKA_VALUE    0x48
#define HMC_CHECKB_VALUE    0x34  
#define HMC_CHECKC_VALUE    0x33  

u8 HMC5883_Check(void);
void Hmc5883_Init(void);








