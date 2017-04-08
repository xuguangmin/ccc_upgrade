/*
 * ccc_packet.c
 *
 *  Created on: 2013-3-28
 *      Author: flx
 */
#include <stdlib.h>
#include <string.h>
#include "ccc_packet.h"


#if 0
int cccpacket_using(CCCPACKET *cccpacket)
{
	if(!cccpacket)
		return 0;
	cccpacket->using = 1;
	return 1;
}
int cccpacket_not_using(CCCPACKET *cccpacket)
{
	if(!cccpacket)
		return 0;
	cccpacket->using = 0;
	return 1;
}

int cccpacket_init(CCCPACKET *cccpacket)
{
	if(!cccpacket)
		return 0;

	cccpacket->using = 0;
	cccpacket->buffer = NULL;
	cccpacket->buffer_size = 0;
	return 1;
}

int cccpacket_release(CCCPACKET *cccpacket)
{
	if(!cccpacket)
		return 0;

	cccpacket->using = 0;
	if(cccpacket->buffer) free(cccpacket->buffer);
	cccpacket->buffer_size = 0;
	return 1;
}

int cccpacket_save_protocol_data(CCCPACKET *cccpacket, const unsigned char *buffer, int data_len)
{
	if(!cccpacket_check_buffer(cccpacket, data_len))
		return 0;
	if(!internal_cccpacket_save_data(cccpacket, buffer, data_len))
		return 0;

	// TODO: FFFFFFFFFFFFFFFFFFFF cccpacket->recv_data.pdu.data     = cccpacket->buffer;
	cccpacket->recv_data.pdu.data_len = cccpacket->data_len;

	cccpacket->data_type = CCCPACKET_DATA_TYPE_PROTOCOL;
	return 1;
}
#endif




static int internal_cccpacket_save_data(CCCPACKET *cccpacket, const unsigned char *buffer, int data_len)
{
	if(!cccpacket || !buffer || data_len <= 0)
		return 0;

	cccpacket->data_len = (data_len > cccpacket->buffer_size) ? cccpacket->buffer_size:data_len;
	memcpy(cccpacket->buffer, buffer, cccpacket->data_len);
	return 1;
}

/* 检查包当前的缓存大小是否足够 */
static int cccpacket_check_buffer(CCCPACKET *cccpacket, int buffer_size)
{
	if(!cccpacket || buffer_size <= 0)
		return 0;

	if(!cccpacket->buffer || cccpacket->buffer_size < buffer_size)
	{
		if(cccpacket->buffer) free(cccpacket->buffer);

		cccpacket->buffer = (unsigned char *)malloc(buffer_size);
		if(!cccpacket->buffer)
			return 0;

		cccpacket->buffer_size = buffer_size;
	}

	return 1;
}

int ccc_packet_save_data_pdu(CCCPACKET *cccpacket, const PDU *pdu)
{
	if(!cccpacket_check_buffer(cccpacket, pdu->data_len))
		return 0;
	if(!internal_cccpacket_save_data(cccpacket, pdu->data, pdu->data_len))
		return 0;

	// TODO: FFFFFFFFFFFFFFFFFFFF cccpacket->recv_data.pdu.data     = cccpacket->buffer;

	cccpacket->data_type = CCCPACKET_DATA_TYPE_PDU;
	cccpacket->un.pdu.data      = cccpacket->buffer;
	cccpacket->un.pdu.data_len  = cccpacket->data_len;
	cccpacket->un.pdu.cmd       = pdu->cmd;
	cccpacket->un.pdu.cmd_ex    = pdu->cmd_ex;
	cccpacket->un.pdu.checksum1 = pdu->checksum1;
	cccpacket->un.pdu.checksum2 = pdu->checksum2;

	return 1;
}

int ccc_packet_save_data(CCCPACKET *cccpacket, const unsigned char *buffer, int data_len)
{
	if(!cccpacket_check_buffer(cccpacket, data_len))
		return 0;
	if(!internal_cccpacket_save_data(cccpacket, buffer, data_len))
		return 0;

	cccpacket->data_type = CCCPACKET_DATA_TYPE_NORMAL;
	return 1;
}

int ccc_packet_malloc(CCCPACKET *cccpacket, int buffer_size)
{
	return cccpacket_check_buffer(cccpacket, buffer_size);
}

void ccc_packet_delete(CCCPACKET *cccpacket)
{
	if(!cccpacket)
		return;

	if(cccpacket->buffer) free(cccpacket->buffer);
	free(cccpacket);
}

CCCPACKET *ccc_packet_create()
{
	CCCPACKET *result = (CCCPACKET *)malloc(sizeof(CCCPACKET));
	if(!result)
		return NULL;

	result->buffer      = NULL;
	result->buffer_size = 0;
	result->data_len = 0;
	return result;
}
