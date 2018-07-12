/*****************************************************************************
*
* File Name : wm_demo_cmd.h
*
* Description: demo command header
*
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd.
* All rights reserved.
*****************************************************************************/
#ifndef __WM_CONSOLE_H__
#define __WM_CONSOLE_H__

#include <string.h>
#include "wm_include.h"
#include "wm_watchdog.h"

/*****************************************************************
	EXTERN FUNC
*****************************************************************/
extern int Relay_control(void *, ...);
extern int LCD_display(void *, ...);
extern int exrtc_save(void *, ...);

#endif /*__WM_CONSOLE_H__*/

