/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名  : udp_server.c
  作   者   : 贾延刚
  生成日期   :

  功能描述   :udp服务

******************************************************************************/
#include <stdio.h>
#include <arpa/inet.h>
#include "util_log.h"
#include "udp_socket.h"
#include "udp_server.h"

static int                  l_sock_udp_server = -1;
struct sockaddr_in          l_remote_addr;		              /* 客户端地址信息 */
static udp_server_callback  l_udp_server_callback  = NULL;

#if 0
static void temp_test(struct sockaddr_in *remote_addr, const uint8_t *buffer, int data_len)
{
	char str[INET_ADDRSTRLEN];
	printf("received from %s at PORT %d\n",
			inet_ntop(AF_INET, &remote_addr->sin_addr, str, sizeof(str)),
			ntohs(remote_addr->sin_port));

	udp_server_send(remote_addr, buffer, data_len);
}
#endif



static void udp_server_output_client_data(struct sockaddr_in *remote_addr, const uint8_t *buffer, int data_len)
{
	if(l_udp_server_callback) l_udp_server_callback(remote_addr, buffer, data_len);
}

static void thread_func_udp_server(int server_sock)
{
	uint8_t buffer[4096];
	while (1)
	{
		int len = udp_recvfrom(server_sock, buffer, 1024, &l_remote_addr);
		if (len <= 0)
		{
			CCC_LOG_OUT("%s udp_recvfrom error\n", __FUNCTION__);
			continue;
		}

		udp_server_output_client_data(&l_remote_addr, buffer, len);
	}
}

int udp_server_send(const void *handle, const uint8_t *buffer, int data_len)
{
	return udp_sendto(l_sock_udp_server, buffer, data_len, (struct sockaddr_in *)handle);
}

int udp_server_start(int port, udp_server_callback callback)
{
	if(!callback)
		return 0;

	l_sock_udp_server = udp_socket();
	if (l_sock_udp_server < 0)
	{
		CCC_LOG_OUT("%s error :udp_socket.\n", __FUNCTION__);
		return 0;
	}

	if (!udp_server_bind(l_sock_udp_server, port))
	{
		CCC_LOG_OUT("%s error :udp_server_bind.\n", __FUNCTION__);
		return 0;
	}

	l_udp_server_callback = callback;
	CCC_LOG_OUT("%s <%d> running\n", __FUNCTION__, port);

	thread_func_udp_server(l_sock_udp_server);
	return 1;
}
