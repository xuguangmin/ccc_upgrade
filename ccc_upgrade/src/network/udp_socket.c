/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名  : udp_socket.c
  作   者   : 贾延刚
  生成日期   :

  功能描述   :socket的封装，实现udp功能


******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udp_socket.h"


int udp_recvfrom(int sock, uint8_t *buffer, int buf_size, struct sockaddr_in *dst_addr)
{
	socklen_t addr_len = sizeof(struct sockaddr_in);
	return recvfrom(sock, buffer, buf_size, 0, (struct sockaddr *)dst_addr, &addr_len);
}

int udp_sendto(int sock, const uint8_t *buffer, int data_len, const struct sockaddr_in *dst_addr)
{
	int bytes_left = 0;
	int first_byte = 0;
	if(sock <= 0 || !buffer || data_len <= 0)
		return 0;

	/*printf("%s :%s %d\n", __FUNCTION__, inet_ntoa(dst_addr->sin_addr), ntohs(dst_addr->sin_port));*/

	bytes_left = data_len;
	while(bytes_left > 0)
	{
		int ret = sendto(sock, &buffer[first_byte], bytes_left, 0, (struct sockaddr*)dst_addr, sizeof(struct sockaddr_in));
		if(ret <= 0) 
			return 0;
			
		bytes_left -= ret;
		first_byte += ret;
	}
	return 1;
}

int udp_server_bind(int sock, int port)
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

	if(bind(sock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1)
	{
		printf("bind error\n");
		return 0;
	}

	return 1;
}

int udp_close(int sock)
{
	if (sock <= 0)
		return 0;

	return (0 == close(sock)) ? 1:0;
}

int udp_socket(void)
{
	return socket(AF_INET, SOCK_DGRAM, 0);
}
