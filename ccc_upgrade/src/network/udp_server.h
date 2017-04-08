/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��  : udp_server.h
  ��   ��   : ���Ӹ�
  ��������   :

  ��������   :udp����

******************************************************************************/
#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

/*
 * ������
 *     handle   ���ͻ��˵ľ��
 *     buffer   ������
 *     data_len �����������ݳ���
 *
 * ����ֵ��
 *      0 �����رտͻ���
 *      1 ���ɹ�
 */
typedef void (*udp_server_callback)(const void *handle, const uint8_t *buffer, int data_len);

extern int udp_server_start(int port, udp_server_callback callback);
extern int udp_server_send(const void *handle, const uint8_t *buffer, int data_len);


#endif /* __UDP_SERVER_H__ */
