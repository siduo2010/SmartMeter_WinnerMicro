#ifndef WM_NETIF2_0_3_H
#define WM_NETIF2_0_3_H

#include "wm_config.h"
#include "wm_type_def.h"
#include "wm_sockets.h"
#include "wm_wifi.h"
#include "wm_params.h"

/** MACRO for callback EVENT to join AP or create soft-AP successfully  */
#define	NETIF_WIFI_JOIN_SUCCESS         0x1
/** MACRO for callback EVENT to fail to join AP */
#define  NETIF_WIFI_JOIN_FAILED          0x2
/** MACRO for callback EVENT to disconnect from AP or destroy soft-AP */
#define	NETIF_WIFI_DISCONNECTED         0x3
/** MACRO for callbck EVENT to get IP address */
#define	NETIF_IP_NET_UP                 0x4
/** MACRO for callback EVNET to create AP successfully */
#define	NETIF_WIFI_SOFTAP_SUCCESS    0x5
/** MACRO for callback EVNET to create soft-AP failed */
#define	NETIF_WIFI_SOFTAP_FAILED     0x6
/** MACRO for callback EVNET to close soft-AP */
#define	NETIF_WIFI_SOFTAP_CLOSED          0x7
/** MACRO for callback EVNET to inform soft ap's net */
#define	NETIF_IP_NET2_UP                0x8

#define NETIF_IPV6_NET_UP           0x9

/** These are the values for ip_addr_t.type */
#define IPADDR_TYPE_V4                0U
#define IPADDR_TYPE_V6                6U
#define IPADDR_TYPE_ANY               46U

#define IPV6_ADDR_MAX_NUM       3       

#if 0
struct ip_addr {
  u32_t addr;
};

typedef struct ip_addr ip_addr_t;	
#endif
#if 0
struct ip4_addr {
  u32_t addr;
};
typedef struct ip4_addr ip4_addr_t;

struct ip6_addr {
  u32_t addr[4];
};
typedef struct ip6_addr ip6_addr_t;

#if (TLS_CONFIG_IPV4 && TLS_CONFIG_IPV6)
typedef struct _ip_addr {
  union {
    ip6_addr_t ip6;
    ip4_addr_t ip4;
  } u_addr;
  u8_t type;
} ip_addr_t;
#else
#if TLS_CONFIG_IPV4
typedef ip4_addr_t ip_addr_t;
#else	  
typedef ip6_addr_t ip_addr_t;
#endif
#endif
#endif
struct tls_ethif {
    ip_addr_t ip_addr;
    ip_addr_t netmask;
    ip_addr_t gw;
#if TLS_CONFIG_IPV6
    ip_addr_t ip6_addr[IPV6_ADDR_MAX_NUM];
#endif
    ip_addr_t dns1;
    ip_addr_t dns2;
    u8 status;              //0:net down; 1:net up
#if TLS_CONFIG_IPV6
    u8 ipv6_status[IPV6_ADDR_MAX_NUM];      //0:net down; 1:net up
#endif
};

//type defination of netif status changed callback.
typedef void (*tls_netif_status_event_fn)(u8 status);

/*************************************************************************** 
* Function: tls_ethernet_init 
* Description: Initialize ethernet. 
* 
* Input: ipcfg: Ip parameters. 
*           wireless_protocol: See #define TLS_PARAM_IEEE80211_XXXX
* 
* Output: None
* 
* Return: 0: Succeed. 
* 
* Date : 2014-6-10 
****************************************************************************/ 
int tls_ethernet_init(void);
/*************************************************************************** 
* Function: tls_netif_get_ethif 
* Description: Get the ip parameters stored in tls_ethif struct.
* 
* Input: None
* 
* Output: None
* 
* Return: Pointer to struct tls_ethif. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
struct tls_ethif * tls_netif_get_ethif(void);
/*************************************************************************** 
* Function: tls_dhcp_start 
* Description: Start DHCP negotiation for a network interface.
* 
* Input: None
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
err_t tls_dhcp_start(void);
/*************************************************************************** 
* Function: tls_dhcp_stop 
* Description: Remove the DHCP client from the interface.
* 
* Input: None
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
err_t tls_dhcp_stop(void);
/*************************************************************************** 
* Function: tls_netif_set_addr 
* Description: Change IP address configuration for a network interface (including netmask
*                   and default gateway).
* 
* Input: ipaddr:      the new IP address
*           netmask:  the new netmask
*           gw:           the new default gateway
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
err_t tls_netif_set_addr(ip_addr_t *ipaddr,
                        ip_addr_t *netmask,
                        ip_addr_t *gw);
/*************************************************************************** 
* Function: tls_netif_set_addr 
* Description: Initialize one of the DNS servers.
* 
* Input: numdns:     the index of the DNS server to set must be < DNS_MAX_SERVERS
*           dnsserver:  IP address of the DNS server to set
* 
* Output: None
* 
* Return: None
* 
* Date : 2014-6-10  
****************************************************************************/ 
void tls_netif_dns_setserver(u8 numdns, ip_addr_t *dnsserver);
/*************************************************************************** 
* Function: tls_netif_set_up 
* Description: Bring an interface up, available for processing traffic.
* 
* Input: None
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
err_t tls_netif_set_up(void);
/*************************************************************************** 
* Function: tls_netif_set_down 
* Description: Bring an interface down, disabling any traffic processing.
* 
* Input: None
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
err_t tls_netif_set_down(void);
/*************************************************************************** 
* Function: tls_netif_add_status_event 
* Description: Add netif status changed callback to event list, if exists, do nothing.
* 
* Input: event_fn: A pointer to tls_netif_status_event_fn.
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
err_t tls_netif_add_status_event(tls_netif_status_event_fn event_fn);
/*************************************************************************** 
* Function: tls_netif_remove_status_event 
* Description: Remove netif status changed callback from event list, if not exists, do nothing.
* 
* Input: event_fn: A pointer to tls_netif_status_event_fn.
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2014-6-10  
****************************************************************************/ 
err_t tls_netif_remove_status_event(tls_netif_status_event_fn event_fn);

struct netif *tls_get_netif(void);

#if TLS_CONFIG_AP
/*************************************************************************** 
* Function: tls_dhcps_start 
* Description: Start DHCP Server for a network interface.
* 
* Input: None
* 
* Output: None
* 
* Return: The DHCP Server error code:
*             DHCPS_ERR_SUCCESS - No error
*             DHCPS_ERR_MEM - Out of memory
*             DHCPS_ERR_LINKDOWN - The NI is inactive
* 
* Date : 2014-6-10  
****************************************************************************/ 
INT8S tls_dhcps_start(void);
/*************************************************************************** 
* Function: tls_dhcps_stop 
* Description: Disable DHCP Server.
* 
* Input: None
* 
* Output: None
* 
* Return: None
* 
* Date : 2014-6-10  
****************************************************************************/ 
void tls_dhcps_stop(void);
/*************************************************************************** 
* Function: tls_dnss_start 
* Description: Start the dns server's service.
* 
* Input: DnsName: Specify the server's dns name
* 
* Output: None
* 
* Return: The DNS Server error code:
*             DNSS_ERR_SUCCESS - No error
*             DNSS_ERR_PARAM - Input parameter error
*             DNSS_ERR_MEM - Out of memory
*             DNSS_ERR_LINKDOWN - The NI is inactive
* 
* Date : 2014-6-10  
****************************************************************************/ 
INT8S tls_dnss_start(INT8U * DnsName);
/*************************************************************************** 
* Function: tls_dnss_stop 
* Description: Stop the dns server's service.
* 
* Input: None
* 
* Output: None
* 
* Return: None
* 
* Date : 2014-6-10  
****************************************************************************/ 
void tls_dnss_stop(void);
/*************************************************************************** 
* Function: tls_dhcps_getip
* Description: Get station's ip address by mac address.
* 
* Input: mac    station's mac address
* 
* Output: None
* 
* Return: ip_addr   station's ip address
* 
* Date : 2015-3-20
****************************************************************************/
ip_addr_t *tls_dhcps_getip(const u8_t *mac);
#endif //TLS_CONFIG_AP

#if TLS_CONFIG_RMMS
/*************************************************************************** 
* Function: tls_rmms_start
* Description: Start remote manager server.
* 
* Input:  None
* 
* Output: None
* 
* Return: The rmms error code:
*             RMMS_ERR_SUCCESS - No error
*             RMMS_ERR_MEM - Out of memory
*             RMMS_ERR_LINKDOWN - The NIF is inactive
* 
* Date : 2015-7-20
****************************************************************************/
INT8S tls_rmms_start(void);
/*************************************************************************** 
* Function: tls_rmms_stop
* Description: Disable remote manager server.
* 
* Input:  None
* 
* Output: None
* 
* Return: None
* 
* Date : 2015-7-20
****************************************************************************/
void tls_rmms_stop(void);
#endif

#if TLS_CONFIG_AP
/*************************************************************************** 
* Function: tls_netif2_set_up
* Description: Bring an interface up, available for processing traffic.
* 
* Input:  None
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2015-3-10
****************************************************************************/
err_t tls_netif2_set_up(void);
/*************************************************************************** 
* Function: tls_netif2_set_down
* Description: Bring an interface down, disabling any traffic processing.
* 
* Input:  None
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2015-3-10
****************************************************************************/
err_t tls_netif2_set_down(void);
/*************************************************************************** 
* Function: tls_netif2_set_addr
* Description: Change IP address configuration for a network interface (including netmask
*                   and default gateway).
* 
* Input: ipaddr:      the new IP address
*        netmask:     the new netmask
*        gw:          the new default gateway
* 
* Output: None
* 
* Return: 0: Succeed; minus: Failed. 
* 
* Date : 2015-3-10
****************************************************************************/
err_t tls_netif2_set_addr(ip_addr_t *ipaddr,
                          ip_addr_t *netmask,
                          ip_addr_t *gw);
/*************************************************************************** 
* Function: tls_dhcps_setdns
* Description: Set dhcp server's dns address.
* 
* Input:  numdns:     the index of the DNS server to set must be < DNS_MAX_SERVERS
* 
* Output: None
* 
* Return: None
* 
* Date : 2015-3-10
****************************************************************************/
void tls_dhcps_setdns(u8_t numdns);
#endif

#endif //WM_NETIF_H
