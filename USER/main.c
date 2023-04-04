#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lsm6dso.h"
#include "bmp280.h"
#include "ap3216c.h"
#include "hmc5883.h"
#include "myiic2.h"
#include "myiic.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小	
#define TASK1_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		3
//任务堆栈大小	
#define TASK2_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
void task2_task(void *pvParameters);

//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

int main(void)
{
	u8 addr_lsm6dso[8];
	u8 addr_bmp280[8];
	u8 addr_ap3216c[8];
	u8 addr_hmc5883[8];
	u8 temp;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	LCD_Init();							//初始化LCD
	IIC_Init();             //init I2C PIN
	IIC2_Init();
	
	POINT_COLOR = RED;
	
	while(LSM6DSO_Check())//检测不到 lsm6dso
	{
		LCD_ShowString(30,10,200,16,16,"lsm6dso Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,10,200,16,16,"Please Check!      ");
		delay_ms(500);
	}
	
	while(BMP280_Check())//检测不到 bmp280
	{
		LCD_ShowString(30,30,200,16,16,"bmp280 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,30,200,16,16,"Please Check!      ");
		delay_ms(500);
	}
	
	while(AP3216C_Check_And_Init())//检测不到 ap3216c
	{
		LCD_ShowString(30,50,200,16,16,"ap3216c Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,50,200,16,16,"Please Check!      ");
		delay_ms(500);
	}
	
	while(HMC5883_Check())//detect hmc5883
	{
		LCD_ShowString(30,70,200,16,16,"hmc5883 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,70,200,16,16,"Please Check!      ");
		delay_ms(500);
	}
	
	//show on LCD
	temp=I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_WHO_AM_I);	
	sprintf((char*)addr_lsm6dso,"lsm6dso WHO_AM_I:0x%02X",temp); 
	LCD_ShowString(30,10,200,16,16,addr_lsm6dso);

	temp=I2C_ReadOneByte(BMP280_ADDRESS,BMP280_CHIPID_REG);	
	sprintf((char*)addr_bmp280,"bmp280 CHIP_ID:0x%02X",temp);
	LCD_ShowString(30,30,200,16,16,addr_bmp280);
	
	temp=I2C_ReadOneByte(AP3216C_ADDR,AP3216C_SYSTEMCONG);	
	sprintf((char*)addr_ap3216c,"AP3216C_SYSTEMCONG:0x%02X",temp); 
	LCD_ShowString(30,50,200,16,16,addr_ap3216c);
	
	temp=I2C2_ReadOneByte(HMC5883_ADDR,HMC_CHEAK_A_REG);	
	sprintf((char*)addr_hmc5883,"HMC5883_CHEAK_A_REG:0x%02X",temp); 
	LCD_ShowString(30,70,200,16,16,addr_hmc5883);
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1 任务函数
void task1_task(void *pvParameters)
{
	u8 temperature[8];
	u8 status=0;
	int16_t temp_raw;
	float temp_deg;
	int16_t acc_x_raw ,acc_y_raw, acc_z_raw, gyr_x_raw, gyr_y_raw, gyr_z_raw;
	float acc_x ,acc_y, acc_z, gyr_x, gyr_y, gyr_z;
	u16 task1_num=0;
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,115,314); 	//画一个矩形	
	LCD_DrawLine(5,130,115,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
	
	//lsm6dso init
	I2C_WriteOneByte(LSM6DSO_ADDRESS,LSM6DSO_CTRL1_XL,ODR_XL_104Hz|FS_XL_2g);
	I2C_WriteOneByte(LSM6DSO_ADDRESS,LSM6DSO_CTRL2_G,ODR_G_104Hz|FS_G_250);
	
	while(1)
	{
		printf("Task1 已经执行：%d次\r\n",task1_num);
		
		status=I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_STATUS_REG);
		
		if(status & TEM_DATA_AVAILABLE)
		{
			temp_raw=I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUT_TEMP_L)|(I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUT_TEMP_H)<<8);
			temp_deg=temp_raw/TEMP_LSB_PER_DEG+TEMP_OFFSET_DEG;
			sprintf((char*)temperature,"temperature:%02f",temp_deg);
			LCD_ShowString(30,90,200,16,16,temperature);		
		}
		
		if(status & GYR_DATA_AVAILABLE)
		{
			gyr_x_raw = I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTX_L_G)|(I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTX_H_G)<<8);
			gyr_y_raw = I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTY_L_G)|(I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTY_H_G)<<8);
			gyr_z_raw = I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTZ_L_G)|(I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTZ_H_G)<<8);

			gyr_x = gyr_x_raw*GYR_LSB_250_PER;
			gyr_y = gyr_y_raw*GYR_LSB_250_PER;
			gyr_z = gyr_z_raw*GYR_LSB_250_PER;
			
			printf("Gyro:X:%02f mdps,Y:%02f mdps,Z:%02f mdps \r\n",gyr_x,gyr_y,gyr_z);
		}
		
		if(status & ACC_DATA_AVAILABLE)
		{
			acc_x_raw=I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTX_L_A)|(I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTX_H_A)<<8);
			acc_y_raw=I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTY_L_A)|(I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTY_H_A)<<8);
			acc_z_raw=I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTZ_L_A)|(I2C_ReadOneByte(LSM6DSO_ADDRESS,LSM6DSO_OUTZ_H_A)<<8);

			acc_x=acc_x_raw*ACC_LSB_2G_PER;
			acc_y=acc_y_raw*ACC_LSB_2G_PER;
			acc_z=acc_z_raw*ACC_LSB_2G_PER;
			
			printf("Acc:X:%02f mg,Y:%02f mg,Z:%02f mg \r\n",acc_x,acc_y,acc_z);			
		}
		
		task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
		LED0=!LED0;
		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//显示任务执行次数
		printf("\r\n");
    vTaskDelay(1000);
	}
}

//task2任务函数
void task2_task(void *pvParameters)
{
	u16 task2_num=0;
	double BMP_Pressure;
	unsigned char i =0;
  unsigned char buf[6];
	unsigned short ir, als, ps;
	int16_t xValue ,yValue, zValue;
	
	POINT_COLOR = BLACK;
	
	Bmp_Init();
	Hmc5883_Init();

	LCD_DrawRectangle(125,110,234,314); //画一个矩形	
	LCD_DrawLine(125,130,234,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
	while(1)
	{
		printf("Task2 已经执行：%d次 \r\n",task2_num);
		
		//bmp280
		while(BMP280_GetStatus(BMP280_MEASURING) != RESET);
		while(BMP280_GetStatus(BMP280_IM_UPDATE) != RESET);
		BMP_Pressure = BMP280_Get_Pressure();
		printf("Pressure %f Pa \r\n",BMP_Pressure);
		
		//ap3216c
    for(i = 0; i < 6; i++)	
    {
        buf[i] = I2C_ReadOneByte(AP3216C_ADDR,AP3216C_IRDATALOW + i);	
    }
    if(buf[0] & 0X80) 
				ir = 0;					
			else 			
				ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03); 			
		als = ((unsigned short)buf[3] << 8) | buf[2];
    if(buf[4] & 0x40)	
				ps = 0;    													
			else 				
				ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);
		printf("ALS:%u ,PS:%u ,IR:%u \r\n",als,ps,ir);
			
		//hmc5883
		xValue = I2C2_ReadOneByte(HMC5883_ADDR,HMC_XLSB_REG)|(I2C2_ReadOneByte(HMC5883_ADDR,HMC_XMSB_REG)<<8);
		zValue = I2C2_ReadOneByte(HMC5883_ADDR,HMC_ZLSB_REG)|(I2C2_ReadOneByte(HMC5883_ADDR,HMC_ZMSB_REG)<<8);
		yValue = I2C2_ReadOneByte(HMC5883_ADDR,HMC_YLSB_REG)|(I2C2_ReadOneByte(HMC5883_ADDR,HMC_YMSB_REG)<<8);
		printf("M-SENSOR:xValue:%d ,yValue:%d ,zValue:%d \r\n",xValue,yValue,zValue);
			
		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
    LED1=!LED1;
		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //显示任务执行次数
		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
		printf("\r\n");
    vTaskDelay(1000);
	}
}

