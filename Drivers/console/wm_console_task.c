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
                    (void *)RelayTaskStk,          /* 任务栈的起始地址 */
                    RELAY_TASK_SIZE * sizeof(u32), /* 任务栈的大小     */
                    RELAY_TASK_PRIO,
                    0);
}

void CreateLCDTask(void)
{
	tls_os_queue_create(&lcd_q, LCD_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL,
			LCD_display,
                    NULL,
                    (void *)LCDTaskStk,          /* 任务栈的起始地址 */
                    LCD_TASK_SIZE * sizeof(u32), /* 任务栈的大小     */
                    LCD_TASK_PRIO,
                    0);
}

void CreateRTCTask(void)
{
	tls_os_queue_create(&rtc_q, RTC_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL,
			exrtc_save,
                    NULL,
                    (void *)RTCTaskStk,          /* 任务栈的起始地址 */
                    RTC_TASK_SIZE * sizeof(u32), /* 任务栈的大小     */
                    RTC_TASK_PRIO,
                    0);
}

void CreateUart1Task(void)
{
	tls_os_queue_create(&uart1_q, UART1_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL, 
			Uart1TaskProc, 
										NULL,
                    (void *)UART1TaskStk,         		/* 任务栈的起始地址 */
                    UART1_TASK_SIZE * sizeof(u32),		 /* 任务栈的大小     */
                    UART1_TASK_PRIO,
                    0);
}


