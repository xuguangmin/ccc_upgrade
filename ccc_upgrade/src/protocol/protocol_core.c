/******************************************************************************

                  版权所有 (C), 2001-2020

 ******************************************************************************
  文件名    ：protocol_core.c
  创建者    ：贾延刚
  生成日期   ：2012-10
  功能描述   : 集控机协议的核心解析函数，从一段字节流中获取一个协议包
             或者根据一些元素，打包一段协议流
  函数列表   :
  修改历史   :

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "util_func.h"
#include "protocol_core.h"


#define PROTOCOL_HEADER                0x7e
#define PROTOCOL_SEG_DATA_LEN          4
#define PROTOCOL_SEG_CHECKSUM_LEN      2

/*
 * 计算有效数据的长度
 * 计算有效数据长度的字节在当前的协议中，占4个字节
 */
int calcu_data_length(unsigned char *data, int size)
{
	if(!data || size < 4)
		return 0;

	return four_bytes_to_int(data, size);
}

/*
 * 计算校验和
 * 累加每个字节，然后只取结果的后两个字节
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


/* 使用pdu的数据，组装一个协议包 */
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
 * 根据给定信息创建一个协议包
 *
 * 参数：
 *      cmd          指令
 *      cmd_ex       扩展指令
 *      data         有效数据
 *      data_len     有效数据长度
 *      out_buffer   保存返回数据的缓存
 *      buffer_size  保存返回数据的缓存大小
 *
 * 返回值：0失败，否则返回协议包的总长度
 * 说明：data如果为NULL或者 data_len <= 0，最后的数据包中有效数据的长度都为0
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
 * used_count 多少数据被使用
 */

/*
 * 解析数据
 *
 *
 * 参数：
 *      in_buffer  输入数据缓存；
 *      data_len   输入数据的长度；
 *
 * 返回值：
 *      已经使用的数据长度。既包含有效数据，也包含被扔掉的无效数据
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
	discard_len = p - in_buffer;                               /* 无效数据的长度 */
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
	if(valid_data_len > 0xFFFFF)                                  /* 如果有效数据长度太大，也是有问题的，扔掉一字节的头0x7E */
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

		return (discard_len + 1);                /* 如果校验通不过，扔掉头0x7E */
	}
	remain_len -= PROTOCOL_SEG_CHECKSUM_LEN;
	lpPdu->b_enable = 1;
	return (data_len - remain_len);
}
