/******************************************************************************

                  ��Ȩ���� (C), 2001-2020

 ******************************************************************************
  �ļ���    ��protocol_core.c
  ������    �����Ӹ�
  ��������   ��2012-10
  ��������   : ���ػ�Э��ĺ��Ľ�����������һ���ֽ����л�ȡһ��Э���
             ���߸���һЩԪ�أ����һ��Э����
  �����б�   :
  �޸���ʷ   :

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "util_func.h"
#include "protocol_core.h"


#define PROTOCOL_HEADER                0x7e
#define PROTOCOL_SEG_DATA_LEN          4
#define PROTOCOL_SEG_CHECKSUM_LEN      2

/*
 * ������Ч���ݵĳ���
 * ������Ч���ݳ��ȵ��ֽ��ڵ�ǰ��Э���У�ռ4���ֽ�
 */
int calcu_data_length(unsigned char *data, int size)
{
	if(!data || size < 4)
		return 0;

	return four_bytes_to_int(data, size);
}

/*
 * ����У���
 * �ۼ�ÿ���ֽڣ�Ȼ��ֻȡ����ĺ������ֽ�
 */
unsigned int calcu_checksum(const unsigned char *data, int size)
{
	int k;
	unsigned int result = 0x0;
	if(!data || size <= 0)
		return 0;

	for(k = 0; k < size; ++k)
	{
		result += data[k];
	}
	return (result & 0xFFFF);
}


/* ʹ��pdu�����ݣ���װһ��Э��� */
void assemble_protocol_packet(const PDU *lpPdu, unsigned char *out_buffer, int *size)
{
	out_buffer[0] = PROTOCOL_HEADER;
	out_buffer[1] = lpPdu->cmd;
	out_buffer[2] = lpPdu->cmd_ex;

	out_buffer[3] = (lpPdu->data_len >> 24) & 0xFF;
	out_buffer[4] = (lpPdu->data_len >> 16) & 0xFF;
	out_buffer[5] = (lpPdu->data_len >> 8) & 0xFF;
	out_buffer[6] = lpPdu->data_len & 0xFF;
	memcpy(out_buffer+7, lpPdu->data, lpPdu->data_len);

	out_buffer[lpPdu->data_len+7] = lpPdu->checksum1;
	out_buffer[lpPdu->data_len+8] = lpPdu->checksum2;
	*size = lpPdu->data_len+9;
}

/*
 * ���ݸ�����Ϣ����һ��Э���
 *
 * ������
 *      cmd          ָ��
 *      cmd_ex       ��չָ��
 *      data         ��Ч����
 *      data_len     ��Ч���ݳ���
 *      out_buffer   ���淵�����ݵĻ���
 *      buffer_size  ���淵�����ݵĻ����С
 *
 * ����ֵ��0ʧ�ܣ����򷵻�Э������ܳ���
 * ˵����data���ΪNULL���� data_len <= 0���������ݰ�����Ч���ݵĳ��ȶ�Ϊ0
 */
unsigned int create_protocol_packet(unsigned char cmd, unsigned char cmd_ex, const unsigned char *data, unsigned int data_len,
		                   unsigned char *out_buffer, unsigned int buffer_size)
{
	int result = 1 + 2 + 4;
	unsigned int checksum;
	if(!out_buffer || buffer_size <= result)
		return 0;

	out_buffer[0] = PROTOCOL_HEADER;
	out_buffer[1] = cmd;
	out_buffer[2] = cmd_ex;

	/* valid data length */
	if(!(data_len > 0 && data))
		data_len = 0;

	out_buffer[3] = (data_len >> 24) & 0xFF;
	out_buffer[4] = (data_len >> 16) & 0xFF;
	out_buffer[5] = (data_len >> 8) & 0xFF;
	out_buffer[6] =  data_len & 0xFF;

	if(data_len > 0 && data)
	{
		memcpy(out_buffer + result, data, data_len);
	}
	result += data_len;

	checksum = calcu_checksum(out_buffer, result);

	out_buffer[result + 0] = (checksum >> 8) & 0xFF;
	out_buffer[result + 1] = checksum & 0xFF;
	result += 2;
	return result;
}

/*
 * used_count �������ݱ�ʹ��
 */

/*
 * ��������
 *
 *
 * ������
 *      in_buffer  �������ݻ��棻
 *      data_len   �������ݵĳ��ȣ�
 *
 * ����ֵ��
 *      �Ѿ�ʹ�õ����ݳ��ȡ��Ȱ�����Ч���ݣ�Ҳ�������ӵ�����Ч����
 */
int parse_packet2(const unsigned char *in_buffer, int data_len, PDU *lpPdu)
{
	int valid_data_len;
	int discard_len, remain_len;
	unsigned int checksum;
	unsigned char *p, *p_start;
	if(!in_buffer || data_len <= 0)
		return 0;

	// find header
	p = memchr(in_buffer, PROTOCOL_HEADER, data_len);
	if(!p)
	{
		printf("discard data 111 : ");
		output_byte_array(in_buffer, data_len, data_len);
		return data_len;
	}

	remain_len = data_len;

	p_start = p;
	discard_len = p - in_buffer;                               /* ��Ч���ݵĳ��� */
	if(discard_len > 0)
	{
		printf("discard data 222 : ");
		output_byte_array(in_buffer, discard_len, discard_len);
		remain_len -= discard_len;
	}

	// header
	if(remain_len <= 1) return discard_len;
	p += 1;
	remain_len -= 1;

	// cmd
	if(remain_len <= 2) return discard_len;
	lpPdu->cmd = *p;
	p++;
	lpPdu->cmd_ex = *p;
	p++;
	remain_len -= 2;

	// length (valid data)
	if(remain_len <= PROTOCOL_SEG_DATA_LEN)
		return discard_len;

	valid_data_len = calcu_data_length(p, PROTOCOL_SEG_DATA_LEN);
	if(valid_data_len > 0xFFFFF)                                  /* �����Ч���ݳ���̫��Ҳ��������ģ��ӵ�һ�ֽڵ�ͷ0x7E */
		return (discard_len + 1);
	lpPdu->data_len = valid_data_len;

	p += PROTOCOL_SEG_DATA_LEN;
	remain_len -= PROTOCOL_SEG_DATA_LEN;

	// valid data
	if(remain_len <= valid_data_len)
		return discard_len;
	memcpy(lpPdu->data, p, valid_data_len);
	p += valid_data_len;
	remain_len -= valid_data_len;

	// checksum
	if(remain_len < PROTOCOL_SEG_CHECKSUM_LEN)
		return discard_len;

	checksum = calcu_checksum(p_start, 1+2+PROTOCOL_SEG_DATA_LEN + valid_data_len);
	lpPdu->checksum1 = (checksum >> 8) & 0xFF;
	lpPdu->checksum2 = checksum & 0xFF;

	if(lpPdu->checksum1 != p[0] || lpPdu->checksum2 != p[1])
	{
		printf("check sum %X error, data length = %d\n", checksum, lpPdu->data_len);
		printf("[0] %X %X\n", checksum & 0xFF, p[0]);
		printf("[1] %X %X\n", (checksum >> 8) & 0xFF, p[1]);

		return (discard_len + 1);                /* ���У��ͨ�������ӵ�ͷ0x7E */
	}
	remain_len -= PROTOCOL_SEG_CHECKSUM_LEN;
	lpPdu->b_enable = 1;
	return (data_len - remain_len);
}
