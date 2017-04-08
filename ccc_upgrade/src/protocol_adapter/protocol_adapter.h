/*****************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 *****************************************************************************
  文件名称   : protocol_adapter.h
  作   者   : 贾延刚
  生成日期   :

  功能描述   : 数据解析、处理等

******************************************************************************/

#ifndef __PROTOCOL_ADAPTER_H__
#define __PROTOCOL_ADAPTER_H__

#include <stdint.h>

extern int protocol_adapter_init(const char *upgrade_dir);
extern int protocol_adapter_process_data(const void *handle, const uint8_t *buffer, int data_len, int b_login);
extern void protocol_adapter_process_udp_data(const void *handle, const uint8_t *buffer, int data_len);

#endif // __PROTOCOL_ADAPTER_H__
