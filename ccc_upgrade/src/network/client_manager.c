/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名   : client_manager.c
  作   者   : 贾延刚
  生成日期   : 2013-12-30
  最近修改   :
  功能描述   :管理客户端连接
             只允许有一个已经登录的连接存在
  修改历史   :

******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "thread_wrapper.h"
#include "network_share.h"
#include "util_log.h"
#include "tcp_socket.h"

#define CLIENT_COUNT_MAX             2
#define CLIENT_IP_LENGTH_MAX         64

typedef struct connect_info
{
	int                b_valid;              /* 这个连接是否有效 */
	int                b_login;              /* 这个连接是否已经登录验证 */
	int                ix;                   /* 在数组中的索引 */
	int                connect_no;

	int                sock;			     /* 套接字 */
	char               ip_address[CLIENT_IP_LENGTH_MAX+1];
	int                ip_port;
}CONNECT_INFO;


static CONNECT_INFO               l_connect_info[CLIENT_COUNT_MAX];
static network_manager_callback   l_network_callback   = NULL;
static pthread_mutex_t            l_connect_info_mutex = PTHREAD_MUTEX_INITIALIZER;

static void client_manager_lock()
{
	pthread_mutex_lock(&l_connect_info_mutex);

}
static void client_manager_unlock()
{
	pthread_mutex_unlock(&l_connect_info_mutex);
}
static int internal_client_manager_close(CONNECT_INFO *conn_arrays, int ix)
{
	if(NULL == conn_arrays || ix < 0 || ix >= CLIENT_COUNT_MAX)
		return 0;

	conn_arrays[ix].b_valid  = 0;
	conn_arrays[ix].b_login  = 0;

	if(conn_arrays[ix].sock > 0) tcp_close(conn_arrays[ix].sock);
	conn_arrays[ix].sock     = -1;
	return 1;
}

static CONNECT_INFO *internal_client_manager_new(CONNECT_INFO *conn_arrays, int sock, const char *ip_addr, int ip_port)
{
	int ix = 0;
	if(NULL == conn_arrays || !ip_addr ||strlen(ip_addr) <= 0)
		return NULL;

	/*
	 * 如果第一个中已经有登录的连接
	 * 则就假定第二个没有
	 */
	if(conn_arrays[0].b_valid && conn_arrays[0].b_login) ix = 1;

	if(strlen(ip_addr) > CLIENT_IP_LENGTH_MAX)
		return NULL;

	strcpy(conn_arrays[ix].ip_address, ip_addr);
	conn_arrays[ix].sock     = sock;
	conn_arrays[ix].ip_port  = ip_port;
	conn_arrays[ix].b_valid  = 1;

	return &conn_arrays[ix];
}

/*
 * 如果一个连接登录，则关掉另一个
 */
static int internal_client_manager_login(CONNECT_INFO *conn_arrays, int ix)
{
	int ix2;
	if(NULL == conn_arrays || ix < 0 || ix >= CLIENT_COUNT_MAX)
		return 0;

	printf("%s login\n", __FUNCTION__);
	if(!conn_arrays[ix].b_valid)                         /* 如果连接已经无效，则不需再做什么了 */
		return 0;

	if(0 == ix) ix2 = 1;
	else        ix2 = 0;
	if(!internal_client_manager_close(conn_arrays, ix2))
		return 0;

	conn_arrays[ix].b_login  = 1;
	return 1;
}

static int internal_client_manager_send(CONNECT_INFO *conn_arrays, int ix, const uint8_t *buffer, int data_len)
{
	if(NULL == conn_arrays || ix < 0 || ix >= CLIENT_COUNT_MAX)
		return 0;

	if(!conn_arrays[ix].b_valid)                         /* 如果连接已经无效，则不需再做什么了 */
		return 0;

	return tcp_send(conn_arrays[ix].sock, buffer, data_len);
}


int client_manager_login(const void *handle)
{
	int result;
	client_manager_lock();
	result = internal_client_manager_login(l_connect_info, (int)handle);
	client_manager_unlock();
	return result;
}
int client_manager_send(const void *handle, const uint8_t *buffer, int data_len)
{
	int result;
	client_manager_lock();
	result = internal_client_manager_send(l_connect_info, (int)handle, buffer, data_len);
	client_manager_unlock();
	return result;
}

static void client_manager_close(int connect_no)
{
	client_manager_lock();
	internal_client_manager_close(l_connect_info, connect_no);
	client_manager_unlock();
}
static CONNECT_INFO *client_manager_new(int sock, const char *ip_addr, int ip_port)
{
	CONNECT_INFO *result;
	client_manager_lock();
	result = internal_client_manager_new(l_connect_info, sock, ip_addr, ip_port);
	client_manager_unlock();
	return result;
}



static int output_client_recv_data(const uint8_t *buffer, int data_len, int connect_no, int b_login)
{
	if(l_network_callback) return l_network_callback((void *)connect_no, buffer, data_len, b_login);  //
	return 0;
}

#define RECV_DATA_BUFFER    4096

static void thread_func_client_pad_or_0(void *param)
{
	int consume;
	uint8_t recBuffer[RECV_DATA_BUFFER];
	uint8_t main_buf[RECV_DATA_BUFFER * 2];
	int iRetrun, recvLen = 0;
	CONNECT_INFO *conn_info = (CONNECT_INFO *)param;
	if(!conn_info)
	{
		CCC_LOG_OUT("error :%s return\n", __FUNCTION__);
		return;
	}

	CCC_LOG_OUT("%s running\n", __FUNCTION__);
    while(1)
    {
		//printf("waiting data ...\n");
		iRetrun = tcp_recv(conn_info->sock, recBuffer, RECV_DATA_BUFFER);

		//printf("= %d\n", iRetrun);
		if (iRetrun > 0)
		{
		 	memcpy(&main_buf[recvLen], recBuffer, iRetrun);
			recvLen += iRetrun;

			consume = output_client_recv_data(main_buf, recvLen, conn_info->ix, conn_info->b_login);

			recvLen -= consume;
			if(recvLen > 0)
			{
				memmove(main_buf, main_buf+consume, recvLen);
			}
		}
		else // iRetrun <= 0
		{
			int sock = conn_info->sock;
			client_manager_close(conn_info->ix);

			CCC_LOG_OUT("client(%d) closed\n\n", sock);
			return;
		}
    }
}

int client_manager_reg(int sock, const char *ip_addr, int ip_port)
{
	CONNECT_INFO *conn_info;
	if(!(conn_info = client_manager_new(sock, ip_addr, ip_port)))
		return 0;

	if(!thread_create((void *)thread_func_client_pad_or_0, conn_info))
	{
		CCC_LOG_OUT("%s thread_create error\n", __FUNCTION__);
		return 0;
	}

	CCC_LOG_OUT("client(%d) connected :%s, %d\n\n", sock, ip_addr, ip_port);
	return 1;
}

int client_manager_init(network_manager_callback callback)
{
	int k;
	for(k = 0; k < CLIENT_COUNT_MAX; ++k)
	{
		l_connect_info[k].ix      = k;
		l_connect_info[k].b_valid = 0;
		l_connect_info[k].b_login = 0;
		l_connect_info[k].sock    = -1;
	}
	l_network_callback = callback;
	return 1;
}
