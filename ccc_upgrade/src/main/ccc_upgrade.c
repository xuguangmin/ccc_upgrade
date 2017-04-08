/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 ******************************************************************************
  文件名称 : ccc_server.c
  作者    : 贾延刚
  生成日期 : 2013-4-23

  版本    : 1.0
  功能描述 : 该文件做为整个程序的核心，连接起来基本所有的功能模块。

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
 * 回调函数
 * 从UDP服务获取数据
 */
void callback_by_udp_server(const void *handle, const uint8_t *buffer, int data_len)
{
	protocol_adapter_process_udp_data(handle, buffer, data_len);
}
/*
 * 回调函数
 * 从连接获取数据
 */
int callback_by_network_manager(const void *handle, const uint8_t *buffer, int data_len, int b_login)
{
	return protocol_adapter_process_data(handle, buffer, data_len, b_login);
}

/*
 * server_port   TCP和UDP服务共用的端口编号
 * upgrade_dir   临时保存上传文件的目录
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
