/*
 * network_share.h
 *
 *  Created on: 2013年12月30日
 *      Author: flx
 */

#ifndef __NETWORK_SHARE_H__
#define __NETWORK_SHARE_H__

/*
 * 参数：
 *     handle   ：客户端的句柄
 *     buffer   ：缓存
 *     data_len ：缓存中数据长度
 *     b_login  ：客户端是否已经被验证
 *
 * 返回值：
 *      buffer中已经使用的数据的数量，按字节
 *
 */
typedef int (*network_manager_callback)(const void *handle, const uint8_t *buffer, int data_len, int b_login);

#endif /* __NETWORK_SHARE_H__ */
