#include "wm_include.h"
#include "wm_drivers.h"

uart1_s user_uart;

#define UART1           1
#define UART_BAUDRATE   4800
#define UART_BUF_SIZE   1024

static s16 Uart1RxCb(u16 len);
static void Uart1RxHandle(void);
static u8 GetCrc(u8 *buf, u16 len);

/*************************************************************************** 
* Description: 串口初始化
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/
void Uart1Init(void)
{
    struct tls_param_uart uart_params;

    wm_uart1_rx_config(WM_IO_PB_11);
		wm_uart1_tx_config(WM_IO_PB_12);

    tls_uart_set_baud_rate(UART1, UART_BAUDRATE);
		tls_uart_set_parity(UART1, TLS_UART_PMODE_ODD);
    tls_param_get(TLS_PARAM_ID_UART, (void *)&uart_params, 1);
    if(uart_params.baudrate != UART_BAUDRATE)
    {
        uart_params.baudrate = UART_BAUDRATE;
				//uart_params.parity = TLS_UART_PMODE_ODD;
        tls_param_set(TLS_PARAM_ID_UART, (void *)&uart_params, 1);
    }
    tls_uart_rx_callback_register(UART1, Uart1RxCb);

    memset((char *)&user_uart, 0, sizeof(uart1_s));
    user_uart.rx_buf = tls_mem_alloc(UART_BUF_SIZE);
    if(user_uart.rx_buf != NULL)
        memset((char *)user_uart.rx_buf, 0, UART_BUF_SIZE);

    user_uart.tx_buf = tls_mem_alloc(UART_BUF_SIZE);
    if(user_uart.tx_buf != NULL)
        memset((char *)user_uart.tx_buf, 0, UART_BUF_SIZE);
    
}
/*************************************************************************** 
* Description: 串口接收回调函数
*
* Auth:
*
*Date: 2018-3-30
****************************************************************************/
static s16 Uart1RxCb(u16 len)
{
//	tls_os_queue_send(gsUserTaskQueue,MSG_UART_RX_DATA, 0);
	Uart1RxHandle();
}
/*************************************************************************** 
* Description: 串口接收数据处理
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/
static void Uart1RxHandle(void)
{
    u8 *cmd_buf = user_uart.rx_buf;
    int ret = 0, i = 0;
    u16 len = 0;
		u8  WR_flag = 0, Check = 0;
    
    if(user_uart.rx_len == 0)
    {
        memset(user_uart.rx_buf, 0, UART_BUF_SIZE);
        if(tls_uart_read(UART1, cmd_buf, 1) > 0)
        {
            if(0xFE == cmd_buf[0])
                user_uart.rx_len = 1;
        }
    }

    if(user_uart.rx_len == 1)
    {
        ret = tls_uart_read(UART1, cmd_buf + user_uart.rx_len, 2 - user_uart.rx_len);

				if(ret > 0)
            user_uart.rx_len += ret;
				
				WR_flag = cmd_buf[1] & 0x0f;
    }

    if(user_uart.rx_len >= 2 && WR_flag == 0x02) //写操作返回指令解析
    {
        ret = tls_uart_read(UART1, cmd_buf + user_uart.rx_len, 2);
        if(ret > 0)
           user_uart.rx_len += ret;
				
				for(i=0;i<3;i++)
					Check += cmd_buf[i];
				Check = (~Check) + 0x33;
				
				if(cmd_buf[3] == Check)
				{
					
					printf("Check is correct!!!\r\n");
				}
				else
				{
					
					printf("Check is wrong!!!\r\n");
				}
				
				user_uart.rx_len = 0;
    } 

		if(user_uart.rx_len >= 2 && WR_flag == 0x01)//读操作返回指令解析
    {
        ret = tls_uart_read(UART1, cmd_buf + user_uart.rx_len, 1);
        if(ret > 0)
           user_uart.rx_len += ret;
				
				len = 4 * cmd_buf[2];
				
				ret = tls_uart_read(UART1, cmd_buf + user_uart.rx_len, len + 1);
        if(ret > 0)
           user_uart.rx_len += ret;
				
				for(i = 0;i < len + 3;i++)
					Check += cmd_buf[i];
				Check = (~Check) + 0x33;
				
				if(cmd_buf[user_uart.rx_len-1] == Check && len <= 4)   //如果只读一个地址Reg，则保存数据
				{
					for(i=0;i<8;i++) meterprotocol.ReadRegBackBuf[i] = cmd_buf[i];
				}
				
				if(cmd_buf[user_uart.rx_len-1] == Check)
				{

					printf("Check is correct!!!\r\n");
				}
				else
				{
					
					printf("Check is wrong!!!\r\n");
				}
					
				user_uart.rx_len = 0;
    } 
}
/*************************************************************************** 
* Description: 用户任务处理回调函数
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/
void Uart1TaskProc(void* param)
{
	Uart1Init();
	tls_os_time_delay(1);
	Meter_init();
	while(CalibVoltage(220))
	{
		printf("Calibrating......");
		Meter_init();
		tls_os_time_delay(50);
	}
/*
	while(CalibCurrent(5))
	{
		printf("Calibrating......");
		Meter_init();
		tls_os_time_delay(50);
	}
*/	
	tls_os_time_delay(100);
	for(;;) 
	{
		ReadVoltage();
		tls_os_time_delay(200);
	}
}