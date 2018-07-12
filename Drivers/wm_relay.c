/*****************************************************************************
*
* File Name : wm_gpio_demo.c
*
* Description: gpio demo function
*
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd.
* All rights reserved.
*
* Author : 
*
* Date : 2014-6-2
*****************************************************************************/
#include "wm_include.h"
#include "wm_drivers.h"

/*************************************************************************** 
* Description: 
*
* Auth: 
*
*Date:  2018-3-30
****************************************************************************/ 
void UserRelayCtrl(u8 status)
{
	u16 relay_pin = WM_IO_PB_18;
	
	tls_gpio_cfg(relay_pin, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
	tls_gpio_write(relay_pin,status);			
}

u8 GetRelayStatus(void)
{
	u16 relay_pin = WM_IO_PB_18;
	
	return tls_gpio_read(relay_pin);
}
//
int Relay_control(void)
{
	while(1)
	{
		
		tls_os_time_delay(0x00000100);/*delay 5 second*/
	}
	return WM_SUCCESS;
}
