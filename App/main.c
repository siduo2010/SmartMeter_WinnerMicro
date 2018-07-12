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
	
	//��ʼ����ģ������
	UserLedInit();
	AT24CXX_init();
	LCD_init();

	//����WiFi����
	CreateWifiTask();
	
	//����ʱ��RTC����
	CreateRTCTask();
	
	//�����̵�������
	CreateRelayTask();
	
	//������ʾ������
	CreateLCDTask();
	
	//��������1(����)����
	CreateUart1Task();
}

