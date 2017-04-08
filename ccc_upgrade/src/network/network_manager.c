/*****************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 *****************************************************************************
  文 件 名   : network_manager.c
  作   者   : 贾延刚
  生成日期   :

  功能描述   : 网络功能的接口
  修改历史   :

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "util_log.h"
#include "tcp_socket.h"
#include "network_share.h"
#include "client_manager.h"


static int l_sock_tcp_server   = -1;


int callback_by_tcp_server_accept(int sock, const char *ip_addr, int port)
{
	if(!client_manager_reg(sock, ip_addr, port))
	{
		CCC_LOG_OUT("%s client_manager_reg error\n", __FUNCTION__);
		return 0;
	}
	return 1;
}


int network_manager_client_send(const void *handle, const uint8_t *buffer, int data_len)
{
	return client_manager_send(handle, buffer, data_len);
}

int network_manager_client_set_login(const void *handle)
{
	return client_manager_login(handle);
}


int tcp_server_start(int tcp_port, network_manager_callback callback)
{
	/* 启动服务 */
	l_sock_tcp_server = tcp_socket();
	if(l_sock_tcp_server < 0)
	{
		CCC_LOG_OUT("%s error :tcp_socket.\n", __FUNCTION__);
		return 0;
	}

	if(!tp_server_bind(l_sock_tcp_server, tcp_port, 256))
	{
		CCC_LOG_OUT("%s error :tp_server_bind\n", __FUNCTION__);
		return 0;
	}

	/* 开始接受客户端连接 */
	if(!tp_server_accept(l_sock_tcp_server, callback_by_tcp_server_accept))
	{
		CCC_LOG_OUT("%s error :tp_server_accept\n", __FUNCTION__);
		return 0;
	}


	client_manager_init(callback);

	CCC_LOG_OUT("%s <%d> running\n", __FUNCTION__, tcp_port);
	return 1;
}
