/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : network_manager.h
  ��   ��   : ���Ӹ�
  ��������   :
  ��������   : ���繦�ܵĽӿ�
  �޸���ʷ   :

******************************************************************************/
#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

#include <stdint.h>
#include "network_share.h"

extern int tcp_server_start(int tcp_port, network_manager_callback callback);
extern int network_manager_client_send(const void *handle, const uint8_t *buffer, int data_len);
extern int network_manager_client_set_login(const void *handle);

#endif /* __NETWORK_MANAGER_H__ */
