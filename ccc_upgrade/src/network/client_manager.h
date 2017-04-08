/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名   : client_manager.h
  作   者   : 贾延刚
  生成日期   : 2013-12-30
  最近修改   :
  功能描述   :管理客户端连接
             只允许有一个已经登录的连接存在
  修改历史   :

******************************************************************************/
#ifndef __CLIENT_MANAGER_H__
#define __CLIENT_MANAGER_H__

#include "network_share.h"

extern int client_manager_init(network_manager_callback callback);
extern int client_manager_reg(int sock, const char *ip_addr, int ip_port);

extern int client_manager_send(const void *handle, const uint8_t *buffer, int data_len);
extern int client_manager_login(const void *handle);

#endif /* __CLIENT_MANAGER_H__ */
