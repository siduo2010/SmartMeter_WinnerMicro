#ifndef __WM_OS_CONFIG_H__
#define __WM_OS_CONFIG_H__
#define OS_CFG_ON  1
#define OS_CFG_OFF 0

#define TLS_OS_UCOS                         OS_CFG_OFF  /*UCOSII  need to modify wm_config.inc*/
#define TLS_OS_FREERTOS                     OS_CFG_ON   /*FreeRTOS need to modify wm_config.inc*/
#endif
