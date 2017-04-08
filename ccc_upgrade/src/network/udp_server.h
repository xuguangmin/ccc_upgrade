/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名  : udp_server.h
  作   者   : 贾延刚
  生成日期   :

  功能描述   :udp服务

******************************************************************************/
#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

/*
 * 参数：
 *     handle   ：客户端的句柄
 *     buffer   ：缓存
 *     data_len ：缓存中数据长度
 *
 * 返回值：
 *      0 ：将关闭客户端
 *      1 ：成功
 */
typedef void (*udp_server_callback)(const void *handle, const uint8_t *buffer, int data_len);

extern int udp_server_start(int port, udp_server_callback callback);
extern int udp_server_send(const void *handle, const uint8_t *buffer, int data_len);


#endif /* __UDP_SERVER_H__ */
