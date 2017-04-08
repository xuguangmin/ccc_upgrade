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

/* Э������ */
typedef struct __CCCPDU
{
	uint8_t     cmd;
	uint8_t     cmd_ex;
	uint8_t    *data;
	int         data_len;
	uint8_t     checksum1;
	uint8_t     checksum2;
	int         b_enable;                        /* ������Ƿ���Ч */
}CCCPDU;


/* һ�������������ݵĻ���
 * Ҳ��һЩ�����ֶ�������һЩ�������
 */
typedef struct __CCCPACKET
{
#define CCCPACKET_DATA_TYPE_NORMAL          0   /* һ���ֽ��� */
#define CCCPACKET_DATA_TYPE_PDU             1   /* Э������ */

	int             data_type;                  /* �������ͣ���������union���ֵ����� */
	union
	{
		PDU         pdu;
	}un;

	uint8_t        *buffer;                     /* �������ݻ��� */
	int             buffer_size;                /* �����С */
	int             data_len;                   /* ���������ݳ��� */


	int             private_data1;              /* ���Ա���һЩ��������ݣ������������ݰ����� */
	int             private_data2;
	int             private_data3;

}CCCPACKET;


extern CCCPACKET *ccc_packet_create();
extern void ccc_packet_delete(CCCPACKET *cccpacket);
extern int  ccc_packet_malloc(CCCPACKET *cccpacket, int buffer_size);


/* �ӿں��� */
extern int ccc_packet_save_data(CCCPACKET *cccpacket, const uint8_t *buffer, int data_len);
extern int ccc_packet_save_data_pdu(CCCPACKET *cccpacket, const PDU *pdu);


#endif /* __CCC_PACKET_H__ */
