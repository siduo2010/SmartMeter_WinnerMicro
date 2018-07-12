#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "wm_include.h"
#include "tls_common.h"
#include "lwip/sockets.h"
#include "airkiss.h"
#include "wm_wifi_oneshot.h"

#if TLS_CONFIG_AIRKISS_MODE_ONESHOT

#define AIRKISS_BSSID_CONNECT_ENABLE      1/* 该功能需要使用带log的库 */


/* 局域网发现 */
#define TLS_CONFIG_AIRKISS_LAN            1

/* airkiss debug switcher */
#define AIRKISS_DEBUG                     0

/* aes-128 key */
#if AIRKISS_ENABLE_CRYPT
#define ONESHOT_AIRKISS_AES_KEY           "winnermicro_wifi"
#endif

/* udp广播包数目 */
#define ONESHOT_AIRKISS_REPLY_CNT_MAX     50

/* udp广播端口 */
#define ONESHOT_AIRKISS_REMOTE_PORT      10000

#define ONESHOT_AIRKISS_SSID_LEN_MAX      32
#define ONESHOT_AIRKISS_PWD_LEN_MAX       64

bool is_airkiss = FALSE;

static u8 random4reply = 0;

/* 0-->12: channel 1-->13 */
#define ONESHOT_AIRKISS_CHANNEL_ID_MIN    0
#define ONESHOT_AIRKISS_CHANNEL_ID_MAX    12


static airkiss_context_t pakcontext[1]  = {0};

#if AIRKISS_DEBUG
#define AIRKISS_PRINT printf
#else
#define AIRKISS_PRINT(s, ...)
#endif

#if AIRKISS_BSSID_CONNECT_ENABLE
static u8 ak_bssid[ETH_ALEN] = {0};
#endif

#if TLS_CONFIG_AIRKISS_LAN
void airkiss_lan_task_create(void);
#endif

void oneshot_airkiss_send_reply(void)
{
    u8 idx;
    int socket_num = 0;
    struct sockaddr_in addr;

    if (is_airkiss)
        is_airkiss = FALSE;
    else
        return;

    socket_num = socket(AF_INET, SOCK_DGRAM, 0);
    AIRKISS_PRINT("create skt %d: send udp broadcast to airkiss.\r\n", socket_num);

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ONESHOT_AIRKISS_REMOTE_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    for(idx = 0; idx < ONESHOT_AIRKISS_REPLY_CNT_MAX; idx++)
    {
        /* 发送结果为包含get_result所得random值的一个字节udp数据包 */
        sendto(socket_num, &random4reply, sizeof(random4reply), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
        if (tls_wifi_get_oneshot_flag()){
            break;
        }else{
            tls_os_time_delay(50);
        }
    }

    closesocket(socket_num);

    return;
}

static void oneshot_airkiss_finish(void)
{
    int ret = -1;
    airkiss_result_t result = {0};

    /* 10.接收完配置数据后从airkiss获取数据来加网 */
    ret = airkiss_get_result(pakcontext, &result);
    if (0 != ret)
    {
        AIRKISS_PRINT("failed to get airkiss result %d.\r\n", ret);
        return;
    }

    AIRKISS_PRINT("start connect: ssid '%s', ssid_len '%hhu', pwd '%s', pwd_len '%hhu', random '%hhu'.\r\n", result.ssid, result.ssid_length, result.pwd, result.pwd_length, result.random);

    random4reply = result.random;
    is_airkiss = TRUE;

    if (tls_oneshot_is_ssid_bssid_match((u8 *)result.ssid, result.ssid_length, ak_bssid))
    {
        ret = tls_wifi_connect_by_ssid_bssid((u8 *)result.ssid, result.ssid_length, ak_bssid, (u8 *)result.pwd, result.pwd_length);
    }
    else
    {
        ret = tls_wifi_connect((u8 *)result.ssid, result.ssid_length, (u8 *)result.pwd, result.pwd_length);
    }
    
    if (WM_SUCCESS != ret)
    {
        AIRKISS_PRINT("failed to connect net, airkiss join net failed.\r\n");
    }
    tls_wifi_set_oneshot_flag(0);

    return;
}


u32 airkiss_chan_cnt = 0;
void tls_oneshot_airkiss_change_channel(void)
{
	airkiss_chan_cnt = 0;
	airkiss_change_channel(pakcontext);
}

#if AIRKISS_BSSID_CONNECT_ENABLE

/* format --> AirKiss recv guide field: length offset %d, mac_sta %02x:%02x:%02x:%02x:%02x:%02x, mac_ap %02x:%02x:%02x:%02x:%02x:%02x */
static int oneshot_airkiss_printf(const char* format, ...)
{
    int i;
    int var;

    char *pos;
    va_list ap;

    va_start(ap, format);

    pos = strstr(format, ", mac_a");/* 旧版本lib可能没有p */
    if (pos)
    {
        for (i = 0; i < (1 + ETH_ALEN); i++)
            var = va_arg(ap, int);
        for (i = 0; i < ETH_ALEN; i++)
        {
            var = va_arg(ap, int);
            ak_bssid[i] = (u8)var;
        }
        va_end(ap);
        va_start(ap, format);
    }

    pos = strstr(format, "mac_crc");
    if (pos)
    {
	airkiss_chan_cnt++;
	va_end(ap);
	va_start(ap, format);
    }

#if AIRKISS_DEBUG
    vprintf(format, ap);
#endif

    va_end(ap);

    return 0;
}
#endif

const static airkiss_config_t akconfig =
{
	(airkiss_memset_fn)&memset,
	(airkiss_memcpy_fn)&memcpy,
	(airkiss_memcmp_fn)&memcmp,
#if AIRKISS_BSSID_CONNECT_ENABLE
    (airkiss_printf_fn)&oneshot_airkiss_printf
#else
#if AIRKISS_DEBUG
    (airkiss_printf_fn)&printf
#else
    NULL
#endif
#endif
};

void tls_airkiss_recv(u8 *data, u16 data_len)
{
    int ret;
   // printf("A1:"MACSTR",A2:"MACSTR",A3:"MACSTR"\n", MAC2STR(data+4),MAC2STR(data+10), MAC2STR(data+16));
    ret = airkiss_recv(pakcontext, data, data_len);
    if (/*(ret == AIRKISS_STATUS_CHANNEL_LOCKED)||*/(airkiss_chan_cnt == 2))/* 8.已经锁定在了当前的信道，此时无需再轮询切换信道监听了 */
    {
		AIRKISS_PRINT("airkiss fix chan.\r\n");
		//tls_oneshot_filter_data(data);
#if 0		
       	if (tls_wifi_get_oneshot_lock_chan() >=0)
       	{
            tls_wifi_stop_oneshot_timer();

#if AIRKISS_BSSID_CONNECT_ENABLE
            ret = tls_wifi_get_chan_by_bssid(ak_bssid);
            if (ret > 0)
            {
                AIRKISS_PRINT("airkiss fix chan = %hhd.\r\n", ret);
                tls_wifi_change_chanel(ret - 1);
            }
            else
#endif
            {
                curr_channel = tls_wifi_get_oneshot_lock_chan();
                AIRKISS_PRINT("airkiss fix base chan = %hhu.\r\n", curr_channel);
            }
	}
#else
	airkiss_chan_cnt = 3;
	tls_oneshot_switch_channel_tim_temp_stop();
#endif
    }
    else if (ret == AIRKISS_STATUS_CHANNEL_LOCKED)
    {
         //printf("mac:"MACSTR",addr:"MACSTR"\n", MAC2STR(data+10), MAC2STR(data+16));
	if (ieee80211_has_fromds(*(u16 *)(data)))
	{
		tls_wifi_set_bcast_mac_filter(data+16, 1,1);
		tls_wifi_set_bcast_mac_filter(data+10, 1,0);
	}else{
		tls_wifi_set_bcast_mac_filter(data+4, 1,1);
		tls_wifi_set_bcast_mac_filter(data+10, 1,0);		
	}
	tls_oneshot_switch_channel_tim_stop();
    }
    else if (ret == AIRKISS_STATUS_COMPLETE)/* 9.已经接收到了所有的配置数据 */
    {
        AIRKISS_PRINT("airkiss recv finish.\r\n");
        tls_wifi_restore_bcast_mac_filter();
        oneshot_airkiss_finish();
    }

    return;
}

void tls_airkiss_start(void)
{
    int ret = -1;

    AIRKISS_PRINT("airkiss version: %s\r\n", airkiss_version());
    AIRKISS_PRINT("start airkiss oneshot config...\r\n");


#if AIRKISS_BSSID_CONNECT_ENABLE
    memset(ak_bssid, 0, ETH_ALEN);
#endif

    ret = airkiss_init(pakcontext, &akconfig);
    if (0 != ret)
    {
        AIRKISS_PRINT("failed to init airkiss.\r\n");
        goto err;
    }
    airkiss_chan_cnt = 0;
    is_airkiss = FALSE;

#if AIRKISS_ENABLE_CRYPT
    ret = airkiss_set_key(pakcontext, ONESHOT_AIRKISS_AES_KEY, strlen(ONESHOT_AIRKISS_AES_KEY));
    if (0 != ret)
    {
        AIRKISS_PRINT("failed to set airkiss aes key.\r\n");
        goto err;
    }
#endif


#if TLS_CONFIG_AIRKISS_LAN
    airkiss_lan_task_create();
#endif

    return;

err:

    return;
}

void tls_airkiss_stop(void)
{
    AIRKISS_PRINT("stop airkiss oneshot config...\r\n");

    return;
}
#endif


#if TLS_CONFIG_AIRKISS_LAN

#define         AIRKISS_LAN_TASK_PRIO             38
#define         AIRKISS_LAN_TASK_STK_SIZE         256
#define         AIRKISS_LAN_TASK_QUEUE_SIZE       16
#define         AIRKISS_LAN_BUF_MAX               256
#define         AIRKISS_LAN_PORT                  12476
#define         AIRKISS_LAN_TIMER                 5

static OS_STK   AirkissLanTaskStk[AIRKISS_LAN_TASK_STK_SIZE];
static bool airkiss_lan_running = FALSE;

static bool airkiss_lan_buf[AIRKISS_LAN_BUF_MAX] = {0};

static int airkiss_lan_select_recv(int skt, u8 *buf, int len, struct sockaddr_in *addr)
{
    int ret;
    socklen_t addrlen;
    fd_set read_set;
    struct timeval tv;

    FD_ZERO(&read_set);
    FD_SET(skt, &read_set);
    tv.tv_sec = AIRKISS_LAN_TIMER;
    tv.tv_usec = 0;

    ret = select(skt + 1, &read_set, NULL, NULL, &tv);
    if (ret > 0)
    {
        if (FD_ISSET(skt, &read_set))
        {
            addrlen = sizeof(struct sockaddr_in);
            ret = recvfrom(skt, buf, len, 0, (struct sockaddr *)addr, &addrlen);

            FD_CLR(skt, &read_set);
        }
        else
        {
            ret = -333;
        }
    }

    return ret;
}

static void airkiss_lan_send(int fd, airkiss_lan_cmdid_t cmd, struct sockaddr_in *addr)
{
    int ret;
    u16 sndlen;

    sndlen = AIRKISS_LAN_BUF_MAX;
    ret = airkiss_lan_pack(cmd, "wechat", "wechat", NULL, 0,
                           airkiss_lan_buf, &sndlen, &akconfig);
    if (AIRKISS_LAN_PAKE_READY == ret)
    {
        addr->sin_port = htons(AIRKISS_LAN_PORT);
        ret = sendto(fd, airkiss_lan_buf, sndlen, 0, (struct sockaddr *)addr, sizeof(struct sockaddr));
        if (ret <= 0)
        {
            AIRKISS_PRINT("airkiss_lan_send = %d\n", ret);
        }
    }
    else
    {
        AIRKISS_PRINT("airkiss_lan_pack = %d\n", ret);
    }

    return;
}

static void airkiss_lan_task(void *data)
{
    int ret;
    int sockfd;
    u32 curr_time = 0;
    u32 last_time = 0;
    struct tls_ethif *ethif;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(AIRKISS_LAN_PORT);

    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    ethif = tls_netif_get_ethif();

    for ( ; ; )
    {
        curr_time = tls_os_get_time();
        if ((curr_time - last_time) >= (AIRKISS_LAN_TIMER * HZ))
        {
            memset(&addr, 0, sizeof(addr));
        	addr.sin_family = AF_INET;
        	addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        	if (ethif->status)
                airkiss_lan_send(sockfd, AIRKISS_LAN_SSDP_NOTIFY_CMD, &addr);
            last_time = curr_time;
        }

        memset(&addr, 0, sizeof(addr));
        ret = airkiss_lan_select_recv(sockfd, airkiss_lan_buf, AIRKISS_LAN_BUF_MAX, &addr);
        if (ret > 0)
        {
            ret = airkiss_lan_recv(airkiss_lan_buf, ret, &akconfig);
            if (AIRKISS_LAN_SSDP_REQ == ret)
            {
                airkiss_lan_send(sockfd, AIRKISS_LAN_SSDP_RESP_CMD, &addr);
            }
            else
            {
                AIRKISS_PRINT("airkiss_lan_recv = %d\n", ret);
            }
        }
    }

}

void airkiss_lan_task_create(void)
{
    if (airkiss_lan_running)
        return;

    tls_os_task_create(NULL, NULL, airkiss_lan_task,
                       (void *)0, (void *)AirkissLanTaskStk,
                       AIRKISS_LAN_TASK_STK_SIZE * sizeof(u32),
                       AIRKISS_LAN_TASK_PRIO, 0);

    airkiss_lan_running = TRUE;

    return;
}

#endif

