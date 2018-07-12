#include "wm_include.h"
#include "wm_drivers.h"

static u8 gsBlinkStatus = 0;		// wifi ״ָ̬ʾ��

/*************************************************************************** 
* Description: �Ƴ�ʼ��
*
* Auth: 
*
*Date:  2018-3-30
****************************************************************************/
void UserLedInit(void)
{
    tls_gpio_cfg(USER_LED_IO, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(USER_LED_IO, 0);
}
/*************************************************************************** 
* Description: ����LED��״̬
*
* Auth: 
*
*Date:  2018-3-30
****************************************************************************/ 
void UserLedCtrl(u8 status)
{
	tls_gpio_write(USER_LED_IO, status);
}
/*************************************************************************** 
* Description: ��ȡ��ǰLED��״̬
*
* Auth: 
*
*Date:  2018-3-30
****************************************************************************/ 
u8 UserLedGetStatus(void)
{
    return tls_gpio_read(USER_LED_IO);
}
/*************************************************************************** 
* Description: ����LED��״̬ȡ��
*
* Auth: 
*
*Date:  2018-3-30
****************************************************************************/
void UserLedOnOff(void)
{
    if(UserLedGetStatus() == 1)
        UserLedCtrl(0);
    else
        UserLedCtrl(1);
}
