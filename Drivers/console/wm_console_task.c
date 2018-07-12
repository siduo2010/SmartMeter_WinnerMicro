/***************************************************************************** 
* 
* File Name : wm_demo_console_task.c
* 
* Description: demo console task
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
*****************************************************************************/ 
#include <string.h>
#include "wm_include.h"
#include "wm_drivers.h"
#include "wm_console.h"

extern int strtodec(int *dec, char *str);

void CreateRelayTask(void)
{
	tls_os_queue_create(&relay_q, RELAY_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL,
			Relay_control,
                    NULL,
                    (void *)RelayTaskStk,          /* ����ջ����ʼ��ַ */
                    RELAY_TASK_SIZE * sizeof(u32), /* ����ջ�Ĵ�С     */
                    RELAY_TASK_PRIO,
                    0);
}

void CreateLCDTask(void)
{
	tls_os_queue_create(&lcd_q, LCD_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL,
			LCD_display,
                    NULL,
                    (void *)LCDTaskStk,          /* ����ջ����ʼ��ַ */
                    LCD_TASK_SIZE * sizeof(u32), /* ����ջ�Ĵ�С     */
                    LCD_TASK_PRIO,
                    0);
}

void CreateRTCTask(void)
{
	tls_os_queue_create(&rtc_q, RTC_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL,
			exrtc_save,
                    NULL,
                    (void *)RTCTaskStk,          /* ����ջ����ʼ��ַ */
                    RTC_TASK_SIZE * sizeof(u32), /* ����ջ�Ĵ�С     */
                    RTC_TASK_PRIO,
                    0);
}

void CreateUart1Task(void)
{
	tls_os_queue_create(&uart1_q, UART1_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL, 
			Uart1TaskProc, 
										NULL,
                    (void *)UART1TaskStk,         		/* ����ջ����ʼ��ַ */
                    UART1_TASK_SIZE * sizeof(u32),		 /* ����ջ�Ĵ�С     */
                    UART1_TASK_PRIO,
                    0);
}


