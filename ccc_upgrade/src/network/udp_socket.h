/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��  : udp_socket.h
  ��   ��   : ���Ӹ�
  ��������   :

  ��������   :socket�ķ�װ��ʵ��udp����


******************************************************************************/
#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include <stdint.h>
#include <netinet/in.h>

extern int udp_socket(void);
extern int udp_close(int sock);
extern int udp_server_bind(int sock, int port);

extern int udp_sendto(int sock, const uint8_t *buffer, int data_len, const struct sockaddr_in *dst_addr);
extern int udp_recvfrom(int sock, uint8_t *buffer, int buf_size, struct sockaddr_in *dst_addr);


#endif /* __UDP_SOCKET_H__ */
