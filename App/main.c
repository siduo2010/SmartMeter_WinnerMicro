/***************************************************************************** 
* 
* File Name : main.c
* 
* Description: main 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : 
* 
* Date : 2014-6-14
*****************************************************************************/ 
#include "wm_include.h"

void UserMain(void)
{
	printf("\n Welcome to use the Samrt Meter made by Neocore!!!\n");
	
	//初始化各模块驱动
	UserLedInit();
	AT24CXX_init();
	LCD_init();

	//创建WiFi任务
	CreateWifiTask();
	
	//创建时钟RTC任务
	CreateRTCTask();
	
	//创建继电器任务
	CreateRelayTask();
	
	//创建显示屏任务
	CreateLCDTask();
	
	//创建串口1(计量)任务
	CreateUart1Task();
}

