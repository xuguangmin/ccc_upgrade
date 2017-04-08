/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 ******************************************************************************
  文件名称   : protocol_adapter.c
  作   者   : 贾延刚
  生成日期   :

  功能描述   : 数据解析、处理等

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "util_log.h"
#include "util_func.h"
#include "configure.h"
#include "packet_pool/packet_pool.h"
#include "protocol/protocol.h"
#include "file_manager/file_manager.h"
#include "network/network_manager.h"
#include "network/udp_server.h"

/*
 * 临时文件夹，保存上传过程的文件
 * 每次上传文件前，需要检查该文件夹是否存在
 */
static char *l_upgrade_temp_dir = NULL;

static void print_pdu_data(int connect_no, PDU *pdu)
{//*
	if(!pdu) return;

	CCC_LOG_OUT("<%d :%02X%02X(%d): ", connect_no, pdu->cmd, pdu->cmd_ex, pdu->data_len);
	if(pdu->data_len > 0) output_byte_array(pdu->data, 2, 2);
//*/
}


static void print_send_data(int connect_no, uint8_t cmd, uint8_t cmd_ex, const uint8_t *data, unsigned int data_len)
{///*
	if(!data) return;

	CCC_LOG_OUT(">%d :%02X%02X(%d) :", connect_no, cmd, cmd_ex, data_len);
	if(data_len > 0) output_byte_array(data, data_len, 2);
//*/
}

static int send_reply_to_udp(const void *handle, uint8_t cmd, uint8_t cmd_ex, const uint8_t *data, unsigned int data_len)
{
	int result = 1;
	CCCPACKET *packet;
	unsigned int packet_size;

	packet = packet_pool_get_buffer(PROTOCOL_DATA_SIZE_MAX);
	if(!packet)
		return 0;

	do{
		packet_size = create_protocol_packet(cmd, cmd_ex, data, data_len, packet->buffer, PROTOCOL_DATA_SIZE_MAX);
		if(packet_size <= 0)
		{
			result = 0;
			break;
		}

		if(!udp_server_send(handle, packet->buffer, packet_size))
		{
			CCC_LOG_OUT("%s failed\n", __FUNCTION__);
			result = 0;
			break;
		}
	}while(0);

	print_send_data(0, cmd, cmd_ex, data, data_len);

	packet_pool_set(packet);
	return result;
}

static int append_data(uint8_t *buffer, const uint8_t *data, int data_len)
{
	if(!buffer || !data || data_len < 0 || data_len > 1024)
		return 0;

	buffer[0] = (data_len >> 8) & 0xFF;
	buffer[1] =  data_len & 0xFF;

	if(data_len > 0 && data)
	{
		memcpy(buffer+2, data, data_len);
	}

	return (data_len+2);
}
static int send_ip(const void *handle)
{
	char hostname[256];
	struct sockaddr_in *sin = NULL;
	struct ifaddrs *ifa = NULL, *ifList;

	if (getifaddrs(&ifList) < 0)
		return 0;

	bzero(hostname, 256);
	if(gethostname(hostname, 255) != 0)
		return 0;

	for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr->sa_family == AF_INET)
		{
			if(0 == strcmp("lo", ifa->ifa_name))
				continue;

			if(ifa->ifa_flags & IFF_RUNNING)
			{
				int len;
				char *ipaddr;
				uint8_t info[2048];
				bzero(info, 2048);

				sin = (struct sockaddr_in *)ifa->ifa_addr;
				ipaddr = inet_ntoa(sin->sin_addr);

				len = append_data(info, (uint8_t *)ipaddr, strlen(ipaddr));
				len +=append_data(info+len, (uint8_t *)hostname, strlen(hostname));

				send_reply_to_udp(handle, PCS_DISCOVER_DEVICE, PCS_EX_01, info, len);
			}
		}
	}
	freeifaddrs(ifList);
	return 1;
}

static int send_reply_to_client(const void *handle, uint8_t cmd, uint8_t cmd_ex, const uint8_t *data, unsigned int data_len)
{
	int result = 1;
	CCCPACKET *packet;
	unsigned int packet_size;

	packet = packet_pool_get_buffer(PROTOCOL_DATA_SIZE_MAX);
	if(!packet)
		return 0;

	do{
		packet_size = create_protocol_packet(cmd, cmd_ex, data, data_len, packet->buffer, PROTOCOL_DATA_SIZE_MAX);
		if(packet_size <= 0)
		{
			result = 0;
			break;
		}

		if(!network_manager_client_send(handle, packet->buffer, packet_size))
		{
			CCC_LOG_OUT("%d %s failed\n", (int)handle, __FUNCTION__);
			result = 0;
			break;
		}
	}while(0);

	print_send_data((int)handle, cmd, cmd_ex, data, data_len);

	packet_pool_set(packet);
	return result;
}

/* 重启设备 */
static int process_protocol_upgrade_cmd_ex_02(const PDU *pdu)
{
	reboot_device();
	return 1;
}

/* 执行脚本 */
static int process_protocol_upgrade_cmd_ex_01(const void *handle, const PDU *pdu)
{
	int send_len;
	unsigned char send_data[8];
	int b_succeeded = 0;
	int error_code = 0;

	if(pdu->data_len > 1)
	{
		char *filetitle;
		int len = pdu->data_len;

		filetitle = (char *)malloc(len + 1);
		memcpy(filetitle, pdu->data, len);
		filetitle[len] = '\0';

		error_code = file_execute_script(l_upgrade_temp_dir, filetitle);
		if(0 == error_code)
		{
			b_succeeded = 1;
		}
		free(filetitle);
	}

	send_data[0] = (!b_succeeded) ? PCS_EX_ERR:PCS_EX_OK;
	send_data[1] = (unsigned char)error_code;
	send_len = 2;

	return send_reply_to_client(handle, PCS_UPGRADE, PCS_EX_01, send_data, send_len);
}
/* 升级 */
static int process_protocol_upgrade(const void *handle, const PDU *pdu)
{
	switch(pdu->cmd_ex)
	{
	case PCS_EX_01 :return process_protocol_upgrade_cmd_ex_01(handle, pdu);  /* 执行脚本 */
	case PCS_EX_02 :return process_protocol_upgrade_cmd_ex_02(pdu);          /* 重启设备 */
	}
	return 0;
}


/* 开始上传过程 */
static int process_protocol_upload_file_cmd_ex_01(const void *handle, const PDU *pdu)
{
	int send_len;
	unsigned char send_data[8];
	int b_succeeded = 0;

	/* 可以做一些初始化的工作 */
	b_succeeded = 1;


	send_data[0] = (!b_succeeded) ? PCS_EX_ERR:PCS_EX_OK;
	send_len = 1;
	return send_reply_to_client(handle, PCS_UPLOAD_FILE, PCS_EX_01, send_data, send_len);
}
/* 收到上传文件名 */
static int process_protocol_upload_file_cmd_ex_02(const void *handle, const PDU *pdu)
{
	int send_len;
	unsigned char send_data[8];
	int b_succeeded = 0;

	if(pdu->data_len > 1)
	{
		char *filetitle;
		int len = pdu->data_len;

		filetitle = (char *)malloc(len + 1);
		memcpy(filetitle, pdu->data, len);
		filetitle[len] = '\0';

		if(file_upload_open(l_upgrade_temp_dir, filetitle))
		{
			b_succeeded = 1;
		}
		free(filetitle);
	}

	send_data[0] = (!b_succeeded) ? PCS_EX_ERR:PCS_EX_OK;
	send_len = 1;

	return send_reply_to_client(handle, PCS_UPLOAD_FILE, PCS_EX_02, send_data, send_len);
}

/* 收到上传数据 */
static int process_protocol_upload_file_cmd_ex_03(const void *handle, const PDU *pdu)
{
	int send_len;
	unsigned char send_data[8];
	int b_succeeded = 0;

	if(pdu->data_len > 4)
	{
		unsigned int offset = four_bytes_to_uint(pdu->data, 4);
		if(file_upload_write((long int)offset, &pdu->data[4], pdu->data_len-4))
		{
			b_succeeded = 1;
		}
	}

	send_data[0] = (!b_succeeded) ? PCS_EX_ERR:PCS_EX_OK;
	send_data[1] = pdu->data[0];
	send_data[2] = pdu->data[1];
	send_data[3] = pdu->data[2];
	send_data[4] = pdu->data[3];
	send_len = 5;

	return send_reply_to_client(handle, PCS_UPLOAD_FILE, PCS_EX_03, send_data, send_len);
}

/* 单个文件上传完成 */
static int process_protocol_upload_file_cmd_ex_04(const void *handle, const PDU *pdu)
{
	int send_len;
	unsigned char send_data[8];
	int b_succeeded = 0;

	if(pdu->data_len > 1 && PCS_EX_OK == pdu->data[0])
	{
		/* 单个文件，做一些验证之类的处理 */
		char *md5sum;
		int len = pdu->data_len-1;

		md5sum = (char *)malloc(len + 1);
		memcpy(md5sum, &pdu->data[1], len);
		md5sum[len] = '\0';
		if(file_upload_single_finish(md5sum))
		{
			b_succeeded = 1;
		}
		free(md5sum);
	}

	send_data[0] = (!b_succeeded) ? PCS_EX_ERR:PCS_EX_OK;
	send_len = 1;
	return send_reply_to_client(handle, PCS_UPLOAD_FILE, PCS_EX_04, send_data, send_len);
}

/* 所有文件上传完成 */
static int process_protocol_upload_file_cmd_ex_05(const void *handle, const PDU *pdu)
{
	unsigned char send_data[8];
	int b_succeeded = 0;

	if(pdu->data_len >= 1 && PCS_EX_OK == pdu->data[0])
	{
		b_succeeded = 1;
	}
	send_data[0] = (!b_succeeded) ? PCS_EX_ERR:PCS_EX_OK;

	return send_reply_to_client(handle, PCS_UPLOAD_FILE, PCS_EX_05, send_data, 1);
}

/* 0xE2 上传文件 */
static int process_protocol_upload_file(const void *handle, const PDU *pdu)
{
	switch(pdu->cmd_ex)
	{
	case PCS_EX_01 :return process_protocol_upload_file_cmd_ex_01(handle, pdu); /* 开始上传过程 */
	case PCS_EX_02 :return process_protocol_upload_file_cmd_ex_02(handle, pdu); /* 收到要上传文件类型和文件名 */
	case PCS_EX_03 :return process_protocol_upload_file_cmd_ex_03(handle, pdu); /* 收到上传数据 */
	case PCS_EX_04 :return process_protocol_upload_file_cmd_ex_04(handle, pdu); /* 单个文件上传完成 */
	case PCS_EX_05 :return process_protocol_upload_file_cmd_ex_05(handle, pdu); /* 上传过程结束 */
	}
	return 0;
}


static int process_protocol_pdu_after_login(const void *handle, const PDU *pdu)
{
	switch(pdu->cmd)
	{
	case PCS_UPLOAD_FILE     :return process_protocol_upload_file(handle, pdu);  /* 0xE2 上传文件 */
	case PCS_UPGRADE         :return process_protocol_upgrade(handle, pdu);      /* 0xE3 升级 */
	default:
		printf("This data packet is bad: unknown command\n");
		return 0;
		break;
	}
	return 1;
}



/* 登录 */
static int process_protocol_client_login(const void *handle, const PDU *pdu)
{
	int send_len;
	int b_succeeded = 0;
	unsigned char send_data[8];

	if(pdu->data_len > 1)
	{
		char *password;
		int len = pdu->data_len;

		password = (char *)malloc(len + 1);
		memcpy(password, pdu->data, pdu->data_len);
		password[len] = '\0';

		if(check_login(password))
		{
			network_manager_client_set_login(handle);
			printf("%s login\n", __FUNCTION__);

			b_succeeded = 1;
		}
		free(password);
	}
	send_data[0] = (!b_succeeded) ? PCS_EX_ERR:PCS_EX_OK;
	send_len = 1;

	return send_reply_to_client(handle, PCS_CLIENT_LOGIN, PCS_EX_01, send_data, send_len);
}

/* 查找集控主机 */
static int process_protocol_discover_device(const void *handle, const PDU *pdu)
{
	send_ip(handle);
	return 1;
}
static int process_protocol_pdu(const void *handle, const PDU *pdu, int b_login)
{
	switch(pdu->cmd)
	{
	case PCS_DISCOVER_DEVICE :return process_protocol_discover_device(handle, pdu);       /* 0xE0 查找集控主机 */
	case PCS_CLIENT_LOGIN    :return process_protocol_client_login(handle, pdu);          /* 0xE1 登录 */
	}

	if(!b_login)
	{
		printf("WARNING :connect <%d>, not login\n", (int)handle);
		return 0;
	}

	return process_protocol_pdu_after_login(handle, pdu);
}


void protocol_adapter_process_udp_data(const void *handle, const uint8_t *buffer, int data_len)
{
	int len;
	CCCPACKET *packet = packet_pool_get_pdu_buf(PROTO_DATA_LEN_MAX);
	if(!packet)
		return;

	PDU *lpPdu = &packet->un.pdu;
	lpPdu->b_enable = 0;

	len = parse_packet2(buffer, data_len, lpPdu);
	if(len <= 0)
		return;

	if(lpPdu->b_enable)
	{
		print_pdu_data(0, lpPdu);
		process_protocol_pdu(handle, lpPdu, 0);
	}
	packet_pool_set(packet);
}

/*
 * 返回值：
 *      已经使用的数据的长度
 */
int protocol_adapter_process_data(const void *handle, const uint8_t *buffer, int data_len, int b_login)
{
	int len, consume = 0;
	CCCPACKET *packet = packet_pool_get_pdu_buf(PROTO_DATA_LEN_MAX);
	if(packet)
	{
		PDU *lpPdu = &packet->un.pdu;
		while(data_len > 0)
		{
			lpPdu->b_enable = 0;
			len = parse_packet2(buffer + consume, data_len, lpPdu);
			if(len <= 0)
				break;

			if(lpPdu->b_enable)
			{
				print_pdu_data((int)handle, lpPdu);
				process_protocol_pdu(handle, lpPdu, b_login);
			}

			data_len -= len;
			consume += len;
		}
		packet_pool_set(packet);
	}
	return consume;
}

int protocol_adapter_init(const char *upgrade_dir)
{
	l_upgrade_temp_dir = strdup(upgrade_dir);
	if(!l_upgrade_temp_dir)
		return 0;

	return 1;
}
