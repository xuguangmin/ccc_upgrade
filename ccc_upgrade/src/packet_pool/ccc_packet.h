/*
 * ccc_packet.h
 *
 *  Created on: 2013-3-28
 *      Author: flx
 */

#ifndef __CCC_PACKET_H__
#define __CCC_PACKET_H__

#include <stdint.h>
#include "protocol/protocol_core.h"

/* 协议数据 */
typedef struct __CCCPDU
{
	uint8_t     cmd;
	uint8_t     cmd_ex;
	uint8_t    *data;
	int         data_len;
	uint8_t     checksum1;
	uint8_t     checksum2;
	int         b_enable;                        /* 这个包是否有效 */
}CCCPDU;


/* 一个用来保存数据的缓存
 * 也有一些其他字段来保存一些别的数据
 */
typedef struct __CCCPACKET
{
#define CCCPACKET_DATA_TYPE_NORMAL          0   /* 一串字节流 */
#define CCCPACKET_DATA_TYPE_PDU             1   /* 协议数据 */

	int             data_type;                  /* 数据类型，用来区分union部分的数据 */
	union
	{
		PDU         pdu;
	}un;

	uint8_t        *buffer;                     /* 保存数据缓存 */
	int             buffer_size;                /* 缓存大小 */
	int             data_len;                   /* 缓存中数据长度 */


	int             private_data1;              /* 可以保存一些额外的数据，并不属于数据包部分 */
	int             private_data2;
	int             private_data3;

}CCCPACKET;


extern CCCPACKET *ccc_packet_create();
extern void ccc_packet_delete(CCCPACKET *cccpacket);
extern int  ccc_packet_malloc(CCCPACKET *cccpacket, int buffer_size);


/* 接口函数 */
extern int ccc_packet_save_data(CCCPACKET *cccpacket, const uint8_t *buffer, int data_len);
extern int ccc_packet_save_data_pdu(CCCPACKET *cccpacket, const PDU *pdu);


#endif /* __CCC_PACKET_H__ */
