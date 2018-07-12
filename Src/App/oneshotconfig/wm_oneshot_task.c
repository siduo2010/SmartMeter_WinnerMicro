#include "wm_osal.h"
#include "tls_sys.h"
#include "wm_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wm_netif.h"
#include "wm_sockets.h"
#include "wm_include.h"
#include "wm_wifi_oneshot.h"
#include "wm_osal.h"
#include "wm_wl_task.h"

#define TLS_ONESHOT_TASK_SIZE      128
#define TLS_ONESHOT_MSG_SIZE           32
tls_os_queue_t *WmOneshotQueue;
OS_STK *WmOneShotTaskStk;
static u8 ucOneshotTaskFlag = 0;

#define TLS_ONESHOT_MSG_SOCKET_CREATE 1

extern bool is_airkiss;

void wm_oneshot_status_event(u8 status )
{
	switch(status)
	{
		case NETIF_IP_NET_UP:
			tls_os_queue_send(WmOneshotQueue,(void *)TLS_ONESHOT_MSG_SOCKET_CREATE,0);
			break;
		default:
			break;
	}
}

#if 1 //TLS_CONFIG_SOCKET_RAW
void wm_oneshot_send_mac(void)
{
	int idx;
	int socket_num = 0;
	u8 mac_addr[8];
	struct tls_socket_desc socket_desc = {SOCKET_CS_MODE_CLIENT};

	socket_desc.cs_mode = SOCKET_CS_MODE_CLIENT;
	socket_desc.protocol = SOCKET_PROTO_UDP;
#if TLS_CONFIG_LWIP_VER2_0_3
	IP_ADDR4(&socket_desc.ip_addr, 255, 255, 255, 255);
#else
	for(idx = 0; idx < 4; idx++){
		socket_desc.ip_addr[idx] = 255;
	}
#endif
	socket_desc.port = 65534;
	socket_num = tls_socket_create(&socket_desc);
	memset(mac_addr,0,sizeof(mac_addr));
	tls_get_mac_addr(mac_addr);
	tls_os_time_delay(50);
	for(idx = 0;idx < 20;idx ++)
	{
		tls_socket_send(socket_num,mac_addr, 6);
		tls_os_time_delay(50);
	}
	tls_socket_close(socket_num);
	socket_num = 0;
}
#endif

static void wm_oneshot_task(void *pdata)
{
	void *msg;

    IGNORE_PARAMETER(pdata);

	for(;;)
	{
		tls_os_queue_receive(WmOneshotQueue, (void **)&msg, 0, 0);
		switch((u32)msg)
		{
			case TLS_ONESHOT_MSG_SOCKET_CREATE:

#if TLS_CONFIG_AIRKISS_MODE_ONESHOT
                if (is_airkiss)
                {
				    oneshot_airkiss_send_reply();
                }
#endif

#if 1//TLS_CONFIG_SOCKET_RAW
                wm_oneshot_send_mac();
#endif

				break;
			default:
				break;
		}
	}

}

int wm_oneshot_task_init(void)
{
	tls_os_status_t err = TLS_OS_ERROR;

	if (ucOneshotTaskFlag)
	{
		return WM_SUCCESS;
	}
	ucOneshotTaskFlag = 1;

	WmOneShotTaskStk = tls_mem_alloc(TLS_ONESHOT_TASK_SIZE*sizeof(u32));
	if (!WmOneShotTaskStk){
		goto FAIL1;
	}

	err = tls_os_queue_create(&WmOneshotQueue,TLS_ONESHOT_MSG_SIZE);
	if (err){
		goto FAIL2;
	}

	err = tls_os_task_create(NULL, NULL,
			wm_oneshot_task,
                    NULL,
                    (void *)WmOneShotTaskStk,
                    TLS_ONESHOT_TASK_SIZE * sizeof(u32),
                    TLS_ONESHOT_TASK_PRIO,
                    0);

	if (err){
		goto FAIL3;
	}

	tls_netif_add_status_event(wm_oneshot_status_event);

	return WM_SUCCESS;

FAIL3:
	tls_os_queue_delete(WmOneshotQueue);
	WmOneshotQueue = NULL;

FAIL2:
	tls_mem_free(WmOneShotTaskStk);
	WmOneShotTaskStk = NULL;

FAIL1:
	ucOneshotTaskFlag = 0;

	return WM_FAILED;
}

