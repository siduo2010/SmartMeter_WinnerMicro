#include "wm_include.h"
#include "wm_drivers.h"

static void KeyScanTimerProc(void *ptmr, void *parg);
static u8 gsKeyPreStatus = 1;
static u8 gsKeyStatus = 1;
static u32 gsKeyCount = 0;		// 长按键计数

static void SockRcvTaskProc(void *sdata);
static void WifiTaskProc(void* param);

static int SocketUdp(void);
static ST_User_Sock_U *user_sock_u =NULL;
static int CreateUdpSock(void);
static void CloseUdpSock(void);
static int CreateUserTask(void);

void CreateWifiTask(void)
{
	tls_os_timer_t *KeyScanTimer = NULL;
	u8 autoconnect;
	
	// check autoconnect
	tls_wifi_auto_connect_flag(WIFI_AUTO_CNT_FLAG_GET, &autoconnect);
	if(WIFI_AUTO_CNT_OFF == autoconnect)
	{
		autoconnect = WIFI_AUTO_CNT_ON;
		tls_wifi_auto_connect_flag(WIFI_AUTO_CNT_FLAG_SET, &autoconnect);
	}	
	// timer	
	tls_os_timer_create(&KeyScanTimer, KeyScanTimerProc, NULL, KEY_SCAN_TIME, TRUE, NULL);
	tls_os_timer_start(KeyScanTimer);
	// create user task
	CreateUserTask();
}

/*************************************************************************** 
* Description: 键盘扫描处理
*
* Auth: 
*
*Date: 
****************************************************************************/ 
static void KeyScanTimerProc(void *ptmr, void *parg)
{
	tls_gpio_cfg(KEY_IO_ONESHOT, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_PULLHIGH);
	gsKeyStatus = tls_gpio_read(KEY_IO_ONESHOT);
	
	//printf("gsKeyStatus:%d\r\n",gsKeyStatus);
	
	if(!gsKeyStatus)
	{
		if(gsKeyPreStatus != gsKeyStatus)
		{
			gsKeyPreStatus = gsKeyStatus;
		}
		else
		{
			gsKeyCount++;
			if(KEY_LONG_COUNT == gsKeyCount)
			{
				tls_os_queue_send(gsUserTaskQueue, (void *)MSG_ONESHOT, 0);
			}
		}
	}
	else
	{
		if(gsKeyPreStatus != gsKeyStatus)
		{
			if(KEY_LONG_COUNT > gsKeyCount)
			{
				tls_os_queue_send(gsUserTaskQueue, (void *)MSG_KEY, 0);
			}
			gsKeyCount = 0;
			gsKeyPreStatus = gsKeyStatus = 1;
		}
	}
	UserLcdBlinkProc();  //scaning wifi connected status
}

/*************************************************************************** 
* Description: 网络状态变化回调函数
*
* Auth: liangm
*
*Date: 2018-3-30
****************************************************************************/ 
static void UserWlanStatusChangedEvent(u8 status)
{
	switch(status)
	{
		case NETIF_WIFI_JOIN_SUCCESS:
			printf("NETIF_WIFI_JOIN_SUCCESS\r\n");
			break;
			
		case NETIF_WIFI_JOIN_FAILED:
			printf("NETIF_WIFI_JOIN_FAILED\r\n");
			break;
			
		case NETIF_WIFI_DISCONNECTED:
			printf("NETIF_WIFI_DISCONNECTED\r\n");
			tls_os_queue_send(gsUserTaskQueue,(void *)MSG_NET_DOWN,0);
			break;
			
		case NETIF_IP_NET_UP:
			printf("NETIF_IP_NET_UP\r\n");
			tls_os_queue_send(gsUserTaskQueue, (void *)MSG_NET_UP, 0);
			break;
			
		default:
			break;
	}
}
/*************************************************************************** 
* Description: 创建用户任务
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/ 
static int CreateUserTask(void)
{
	tls_os_queue_create(&gsUserTaskQueue, WIFI_QUEUE_SIZE);
	tls_os_task_create(NULL, NULL, 
					WifiTaskProc, 
										NULL,
                    (void *)WifiTaskStk,         			 /* 任务栈的起始地址 */
                    WIFI_TASK_SIZE * sizeof(u32),		 /* 任务栈的大小     */
                    WIFI_TASK_PRIO,
                    0);

    //用于socket数据的接收
    tls_os_task_create(NULL, NULL, 
					SockRcvTaskProc,
                (void *)user_sock_u,
                sock_rcv_task_stk,          /* 任务栈的起始地址 */
                SOCK_TASK_SIZE, /* 任务栈的大小     */
                SOCKET_RCV_TASK_PRIO,
                0);
    
	return WM_SUCCESS;
}
/*************************************************************************** 
* Description: 用户任务处理回调函数
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/ 
static void WifiTaskProc(void* param)
{
	void *msg;
	u8 oneshotflag = 0;
	
// 注册网络状态回调函数	
	tls_netif_add_status_event(UserWlanStatusChangedEvent);
	for(;;) 
	{
		tls_os_queue_receive(gsUserTaskQueue, (void **)&msg, 0, 0);
		printf("msg = %x\r\n", msg);
		switch((u32)msg)
		{
			case MSG_NET_UP:				// 加网成功
				if(oneshotflag)
				{
          oneshotflag = 0;
					oneshot_airkiss_send_reply();
				}
				UserLcdBlink(0);
				UserLcdCtrl(1);
#if 1	
				{
					struct tls_ethif *ethif;
					u8 *ip;
					
					ethif = tls_netif_get_ethif();
					ip = &ethif->ip_addr.addr;
					printf("ip = %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
				}
#endif			
        SocketUdp();
                    
				break;
				
			case MSG_NET_DOWN:				// 网络断开
				UserLcdCtrl(0);
        CloseUdpSock();
				break;
				
			case MSG_ONESHOT:				// 启动一键配置
				tls_wifi_set_oneshot_flag(1);	
				oneshotflag = 1;
				UserLcdBlink(1);
				break;

      case MSG_SOCKET_SND:
        int ret=0;
        struct sockaddr_in pin;
        if(user_sock_u->tx_len > 0)
        {
            memset(&pin, 0, sizeof(struct sockaddr_in));
            pin.sin_family=AF_INET;                 //AF_INET表示使用IPv4
            pin.sin_addr.s_addr = user_sock_u->remote_ip;
            pin.sin_port = user_sock_u->remote_port;

            ret=sendto(user_sock_u->socket_num, user_sock_u->sock_tx, user_sock_u->tx_len,
                        0, (struct sockaddr*)&pin, sizeof(struct sockaddr));
                    
            if(ret >= 0)
            {
                user_sock_u->tx_len -= ret;
                if(user_sock_u->tx_len > 0)
                   tls_os_queue_send(gsUserTaskQueue, (void *)MSG_SOCKET_SND, 0);
            }
         }
         break;
                
       case MSG_SOCKET_ERR:
         struct tls_ethif * ethif = tls_netif_get_ethif();
         if(ethif->status)
         {
            tls_os_time_delay(HZ);
            SocketUdp();
         }
                
         break;

			case MSG_KEY:
						
				break;
				
			default:
				break;
		}
	}
}
/*************************************************************************** 
* Description: UDP接收任务处理回调函数
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/ 
static void SockRcvTaskProc(void *sdata)
{
    ST_User_Sock_U *sock_u = NULL;
    int ret = 0;
    struct sockaddr_in pin;
    socklen_t addrlen = sizeof(struct sockaddr);
    u32 cnt=0;
		
    for(;;)
    {
        sock_u = user_sock_u;
        if (NULL != sock_u && sock_u->conn_status == 1)
        {
            memset(sock_u->sock_rx, 0, USER_SOCK_BUF_SIZE+1);
            ret = recvfrom(sock_u->socket_num , sock_u->sock_rx, USER_SOCK_BUF_SIZE, 
                    0, (struct sockaddr*)&pin, &addrlen);
					
						Data_Handle(sock_u->sock_rx,ret);	//接收上位机数据并处理function
					
            if (ret > 0)
            {
                sock_u->rx_len += ret;
                printf("rcv from %s  : port : %d len = %d total : %d\n",inet_ntoa(pin.sin_addr), htons(pin.sin_port), ret, sock_u->rx_len);				
							
								//printf("\r\n\r\nfei chang de qiguai:%x\r\n\r\n",UDPAck[1]);
								memcpy(sock_u->sock_tx, &UDPAck, UDPAckLen);
                sock_u->tx_len = UDPAckLen;
                sock_u->remote_ip = pin.sin_addr.s_addr;
                sock_u->remote_port = pin.sin_port;
								tls_os_queue_send(gsUserTaskQueue, (void *)MSG_SOCKET_SND, 0);
								memset(UDPAck, 0, UDPAckLen);
            }
            else
            {
                printf("rcv err\n");
                closesocket(sock_u->socket_num);
                printf("closesocket\n");
                sock_u->socket_num = -1;
                user_sock_u->conn_status = 0;
                tls_os_queue_send(gsUserTaskQueue, (void *)MSG_SOCKET_ERR, 0);
            }
            continue;
        }
        tls_os_time_delay(HZ/10);
    }
}
/*************************************************************************** 
* Description: 分配空间 创建UDP
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/ 
static int SocketUdp(void)
{
    if(NULL == user_sock_u)
    {
        user_sock_u = tls_mem_alloc(sizeof(ST_User_Sock_U));
        if (NULL == user_sock_u)
            goto _error;
        memset(user_sock_u, 0, sizeof(ST_User_Sock_U));

        user_sock_u->sock_rx = tls_mem_alloc(USER_SOCK_BUF_SIZE + 1);
        if (NULL == user_sock_u->sock_rx)
            goto _error;
        memset(user_sock_u->sock_rx, 0, USER_SOCK_BUF_SIZE + 1);

        user_sock_u->sock_tx = tls_mem_alloc(USER_SOCK_BUF_SIZE + 1);
        if (NULL == user_sock_u->sock_tx)
            goto _error;
        memset(user_sock_u->sock_tx, 0, USER_SOCK_BUF_SIZE + 1);

        user_sock_u->socket_num = -1;
        user_sock_u->conn_status = 0;
    }

    return CreateUdpSock();

_error:
    if(NULL != user_sock_u)
    {
        if(NULL != user_sock_u->sock_tx)
            tls_mem_free(user_sock_u->sock_tx);
        if(NULL != user_sock_u->sock_rx)
            tls_mem_free(user_sock_u->sock_rx);
        tls_mem_free(user_sock_u);
        user_sock_u = NULL;
    }
    
    return WM_FAILED;
}
/*************************************************************************** 
* Description: 创建UDP
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/ 
static int CreateUdpSock(void)
{
    struct sockaddr_in pin;
    int ttl=10, loop=0;

    if((user_sock_u->socket_num = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == -1)
    {
        printf("socket err\n");
        goto error;
    }

    memset(&pin, 0, sizeof(struct sockaddr));
    pin.sin_family=AF_INET;                 //AF_INET表示使用IPv4
    pin.sin_addr.s_addr = (u32)0x00000000UL;        //IPADDR_ANY
    pin.sin_port=htons(UDP_LOCAL_PORT);

    printf("\nsocket num=%d\n", user_sock_u->socket_num);
    printf("local port = %d\n", UDP_LOCAL_PORT);
    if (bind(user_sock_u->socket_num, (struct sockaddr *) &pin, sizeof(pin)) == -1)
    {
        printf("bind err\n");
        goto error;
    }

    user_sock_u->conn_status = 1;
    
    return WM_SUCCESS;

error:
    if(user_sock_u->socket_num >= 0)
    {
        closesocket(user_sock_u->socket_num);
        user_sock_u->socket_num = -1;
        user_sock_u->conn_status = 0;
    }
    tls_os_queue_send(gsUserTaskQueue, (void *)MSG_SOCKET_ERR, 0);
    return WM_FAILED;

}
/*************************************************************************** 
* Description: 关闭UDP
*
* Auth: 
*
*Date: 2018-3-30
****************************************************************************/
static void CloseUdpSock(void)
{
    if(user_sock_u->socket_num >= 0)
    {
        closesocket(user_sock_u->socket_num);
        user_sock_u->socket_num = -1;
        user_sock_u->conn_status = 0;
    }
}
