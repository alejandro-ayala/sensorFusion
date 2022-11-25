#pragma once

#include "netif/xadapter.h"
#include "../Common/platform_config.h"
#include "xil_printf.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "lwipopts.h"
#include "lwip/dhcp.h"
#if LWIP_IPV6==0
#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif

static struct netif server_netif;


void lwip_init();
void print_ip(char *msg, ip_addr_t *ip);
void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);
void network_thread(void *p);
