/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �ļ����� : ccc_server.c
  ����    : ���Ӹ�
  �������� : 2013-4-23

  �汾    : 1.0
  �������� : ���ļ���Ϊ��������ĺ��ģ����������������еĹ���ģ�顣

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "util_log.h"
#include "packet_pool/packet_pool.h"
#include "network/network_manager.h"
#include "network/udp_server.h"
#include "protocol_adapter/protocol_adapter.h"

/*
 * �ص�����
 * ��UDP�����ȡ����
 */
void callback_by_udp_server(const void *handle, const uint8_t *buffer, int data_len)
{
	protocol_adapter_process_udp_data(handle, buffer, data_len);
}
/*
 * �ص�����
 * �����ӻ�ȡ����
 */
int callback_by_network_manager(const void *handle, const uint8_t *buffer, int data_len, int b_login)
{
	return protocol_adapter_process_data(handle, buffer, data_len, b_login);
}

/*
 * server_port   TCP��UDP�����õĶ˿ڱ��
 * upgrade_dir   ��ʱ�����ϴ��ļ���Ŀ¼
 *
 */
int start_ccc(int server_port, const char *upgrade_dir)
{
	if(!protocol_adapter_init(upgrade_dir))
	{
		CCC_LOG_OUT("%s, error :protocol_adapter_init.\n", __FUNCTION__);
		return 0;
	}

	if(!tcp_server_start(server_port, callback_by_network_manager))
	{
		CCC_LOG_OUT("%s, error :tcp_server_start.\n", __FUNCTION__);
		return 0;
	}

	if(!udp_server_start(server_port, callback_by_udp_server))
	{
		CCC_LOG_OUT("%s, error :udp_server_start.\n", __FUNCTION__);
		return 0;
	}
	return 1;
}
