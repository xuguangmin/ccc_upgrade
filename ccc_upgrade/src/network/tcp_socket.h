/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名  : tcp_socket.h
  作   者   : 贾延刚
  生成日期   :

  功能描述   :socket的封装，实现tcp功能


******************************************************************************/
#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__


#include <stdio.h>
#include <string.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


/*
 * 参数：
 *     sock     ：客户端的句柄
 *     ip_addr  ：客户端IP
 *     port     ：客户端端口
 *
 * 返回值：
 *      0 ：将关闭客户端
 *      1 ：成功
 */
typedef int (*tcp_server_accept_callback)(int sock, const char *ip_addr, int port);

extern int tcp_socket(void);
extern int tcp_close(int sock);

extern int tp_server_bind(int sock, int port, int maximum);
extern int tp_server_accept(int sock, tcp_server_accept_callback callback);

extern int tcp_send(int sock, const uint8_t *buffer, int data_len);
extern int tcp_send_async(int sock, const uint8_t *buffer, int data_len);
extern int tcp_recv(int sock, uint8_t *buffer, int buf_size);

#endif /* __TCP_SOCKET_H__ */
