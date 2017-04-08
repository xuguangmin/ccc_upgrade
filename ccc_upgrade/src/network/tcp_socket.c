/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名  : tcp_socket.c
  作   者   : 贾延刚
  生成日期   :

  功能描述   :socket的封装，实现tcp功能


******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "tcp_socket.h"
#include "thread_wrapper.h"


typedef struct __ACCEPT_THREAD_PARAM
{
     int                        sock;
     tcp_server_accept_callback callback;
}ACCEPT_THREAD_PARAM;


static void *thread_func_accept_client(void * param)
{
	char ip_addr[256];
	int client_fd;
	struct sockaddr_in client_addr;		                    /* 客户端地址信息 */
	socklen_t sin_size = sizeof(struct sockaddr_in);

	ACCEPT_THREAD_PARAM *threadParam = (ACCEPT_THREAD_PARAM *)param;
	if(!threadParam || !threadParam->callback)
		return ((void*)0);

	while (1)
	{
		 client_fd = accept(threadParam->sock, (struct sockaddr *)&client_addr, &sin_size);
		 if (client_fd < 0)
		 {
			printf("%s error accept\n", __FUNCTION__);
			continue;
		 }

		 bzero(ip_addr, 256);
		 strncpy(ip_addr, inet_ntoa(client_addr.sin_addr), 255);
 		 if(!threadParam->callback(client_fd, ip_addr, ntohs(client_addr.sin_port)))
 		 {
 			 /* 如果返回不成功值，关闭该sock*/
 			close(client_fd);
 		 }

	}
	free(threadParam);
	return ((void*)0);
}

/*
 * 服务socket
 * 启动一个线程，接受客户端的连接
 */
int tp_server_accept(int sock, tcp_server_accept_callback callback)
{
	ACCEPT_THREAD_PARAM *params = (ACCEPT_THREAD_PARAM *) malloc(sizeof(ACCEPT_THREAD_PARAM));
	if(!params)
		return 0;

	params->sock = sock;
	params->callback = callback;

	if(!thread_create((void *)thread_func_accept_client, (void *)params))
	{
		printf("%s error thread_create %s\n", __FUNCTION__, strerror(errno));
		return 0;
	}
	return 1;
}
/*
 * 服务socket绑定一个端口
 */
int tp_server_bind(int sock, int port, int maximum)
{
	struct sockaddr_in server_addr;
	int sock_reuse = 1;
	if (maximum <= 0)
		return 0;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sock_reuse,
			sizeof(sock_reuse)) != 0)
	{
		printf("%s socket reuse failed\n", __FUNCTION__);
		return 0;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons((uint16_t) port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) != 0)
	{
		printf("%s error :socket bind\n", __FUNCTION__);
		return 0;
	}

	if (listen(sock, maximum) != 0)
	{
		printf("%s error :socket listen\n", __FUNCTION__);
		return 0;
	}
	return 1;
}

int tcp_recv(int sock, uint8_t *buffer, int buf_size)
{
	return recv(sock, buffer, buf_size, 0);
}

int tcp_send(int sock, const uint8_t *buffer, int data_len)
{
	int bytes_left = 0;
	int first_byte = 0;
	if(sock <= 0 || !buffer || data_len <= 0)
		return 0;
		
	bytes_left = data_len;
	while(bytes_left > 0)
	{
		int ret = send(sock, &buffer[first_byte], bytes_left, 0);
		if(ret <= 0) 
			return 0;
			
		bytes_left -= ret;
		first_byte += ret;
	}
	return 1;
}


int tcp_send_async(int sock, const uint8_t *buffer, int data_len)
{
	int sended = 0;
	if(sock <= 0 || !buffer || data_len <= 0)
		return 0;

	while(data_len > 0)
	{
		int len = send(sock, &buffer[sended], data_len, 0);
		if(len <= 0)
		{
			if(len == 0)
			{
				return 0;
			}
			else{
				if (errno != EAGAIN)
					return 0;
			}
			continue;
		}


		data_len -= len;
		sended += len;
	}
	//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>  send = %d\n\n", sended);
	return 1;
}


int tcp_close(int sock)
{
	if (sock <= 0)
		return 0;

	return (0 == close(sock)) ? 1:0;
}

int tcp_socket(void)
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}
