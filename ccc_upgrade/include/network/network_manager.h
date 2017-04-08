/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名   : network_manager.h
  作   者   : 贾延刚
  生成日期   :
  功能描述   : 网络功能的接口
  修改历史   :

******************************************************************************/
#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

#include <stdint.h>
#include "network_share.h"

extern int tcp_server_start(int tcp_port, network_manager_callback callback);
extern int network_manager_client_send(const void *handle, const uint8_t *buffer, int data_len);
extern int network_manager_client_set_login(const void *handle);

#endif /* __NETWORK_MANAGER_H__ */
