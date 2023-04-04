实验器材:
	战舰 STM32F103 V3 开发板
	
实验目的:
	使用 stm32 + FreeRTOS，收集四个传感器的数据，实现一个 sensorhub。
	
硬件资源:
	串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面) 
	
实验现象:
	实验中会创建两个应用任务task1_task和task2_task。
