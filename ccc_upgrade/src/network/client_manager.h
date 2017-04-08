/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : client_manager.h
  ��   ��   : ���Ӹ�
  ��������   : 2013-12-30
  ����޸�   :
  ��������   :����ͻ�������
             ֻ������һ���Ѿ���¼�����Ӵ���
  �޸���ʷ   :

******************************************************************************/
#ifndef __CLIENT_MANAGER_H__
#define __CLIENT_MANAGER_H__

#include "network_share.h"

extern int client_manager_init(network_manager_callback callback);
extern int client_manager_reg(int sock, const char *ip_addr, int ip_port);

extern int client_manager_send(const void *handle, const uint8_t *buffer, int data_len);
extern int client_manager_login(const void *handle);

#endif /* __CLIENT_MANAGER_H__ */
