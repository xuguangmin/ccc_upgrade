/*****************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 *****************************************************************************
  �ļ�����   : protocol_adapter.h
  ��   ��   : ���Ӹ�
  ��������   :

  ��������   : ���ݽ����������

******************************************************************************/

#ifndef __PROTOCOL_ADAPTER_H__
#define __PROTOCOL_ADAPTER_H__

#include <stdint.h>

extern int protocol_adapter_init(const char *upgrade_dir);
extern int protocol_adapter_process_data(const void *handle, const uint8_t *buffer, int data_len, int b_login);
extern void protocol_adapter_process_udp_data(const void *handle, const uint8_t *buffer, int data_len);

#endif // __PROTOCOL_ADAPTER_H__
