/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��  : tcp_socket.h
  ��   ��   : ���Ӹ�
  ��������   :

  ��������   :socket�ķ�װ��ʵ��tcp����


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
 * ������
 *     sock     ���ͻ��˵ľ��
 *     ip_addr  ���ͻ���IP
 *     port     ���ͻ��˶˿�
 *
 * ����ֵ��
 *      0 �����رտͻ���
 *      1 ���ɹ�
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
