/*
 * packet_pool.h
 *
 *  Created on: 2013-3-27
 *      Author: flx
 */

#ifndef __PACKET_POOL_H__
#define __PACKET_POOL_H__

#include "ccc_packet.h"


extern void packet_pool_size_max(int size);


extern CCCPACKET *packet_pool_get();
extern CCCPACKET *packet_pool_get_buffer(int buffer_size);
extern CCCPACKET *packet_pool_get_pdu_buf(int buffer_size);

extern int packet_pool_set(CCCPACKET *pkt_ptr);

#endif /* __PACKET_POOL_H__ */
