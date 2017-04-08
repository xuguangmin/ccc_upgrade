#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>

#include "flxdefines.h"
#include "tcpserver.h"
#include "flxthread.h"
#include "flxnettypes.h"

#if defined(FLXLINUX) || defined(FLXUNIX)
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif

/* jia static pthread_mutex_t sendMessageLock=PTHREAD_MUTEX_INITIALIZER; */

//////////////////////////////////////////////////////////////////////////
//   函数名称：tcp_server_acceptClientCallBack
//   功能：接受客户端连接回调函数
//   参数：
//        serverSock:FlxSocket，客户端套接字
//        endpoint:FLXEndPoint *，客户端
//   返回值：
//       如果发送成功，则返回0,否则返回-1
/////////////////////////////////////////////////////////////////////////
//typedef FLXInt32 (*tcp_server_acceptClientCallBack)(FLXSocket *sock,FLXIpEndPoint* ipEndpoint);

////////////////////////////////////////////////////////////////////////////////////
//接受客户端线程参数类型
////////////////////////////////////////////////////////////////////////////////////
typedef struct acceptClientThreadParam
{
     FLXSocket serverSock;
     void * acceptClientCallBack;//FLXInt32 (*tcp_server_acceptClientCallBack)(FLXSocket *sock,FLXIpEndPoint* ipEndpoint);接受到客户端
}AcceptClientThreadParam, *PAcceptClientThreadParam;

////////////////////////////////////////////////////////////////////////////
//   函数名称:tcp_server_initialize
//   功能：初始化Tcp服务器端组件
//   参数：无
//   返回值：
//       如果初始化成功，则返回已初始化的套接字,否则返回-1
///////////////////////////////////////////////////////////////////////////
FLXSocket tcp_server_initialize(void)
{
		FLXSocket iSocket;
	
#ifdef FLXWIN
		WSADATA wsaData;
		FLXInt32 iResult = WSAStartup(MAKEWORD(2,2), &wsaData); 	//开始调用WS2_32.DLL
		if (iResult != NO_ERROR)
		{
			TCP_SERVER_LIB_DEBUG(("Error at WSAStartup()\n"));
		}
#endif
	
		iSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (iSocket == INVALID_SOCKET)
		{
			TCP_SERVER_LIB_DEBUG(("Create socket error "));
#ifdef FLXWIN
			WSACleanup();
#endif
			return -1;
		}
		else
		{
			return iSocket;
		}
}
static void *acceptClientThread(void * param)
{
	int ipPort;
	char ipAddress[16];
	struct sockaddr_in remote_addr;		/* 客户端地址信息 */
	FLXSocket serverSock, client_fd;
	AcceptClientThreadParam *threadParam;
	tcp_server_acceptClientCallBack callback;
	FLXUnint32 sin_size = sizeof(struct sockaddr_in);

	threadParam = (AcceptClientThreadParam *)param; 
	callback = (tcp_server_acceptClientCallBack)threadParam->acceptClientCallBack;
	if(!callback)
		return ((void*)0);

	serverSock = threadParam->serverSock;
	while (1)
	{
		 client_fd = accept(serverSock, (struct sockaddr *)&remote_addr, &sin_size);
		 if (client_fd  == -1) 
		 {
			printf("accept error\n");
			continue;
		 }

		 ipPort = ntohs(remote_addr.sin_port);
		 strcpy(ipAddress, inet_ntoa(remote_addr.sin_addr));
 		 if(callback(client_fd, ipAddress, ipPort) != 0)   /* 如果返回不成功值，关闭该sock*/
 			 close(client_fd);
	}
	return ((void*)0);
}

////////////////////////////////////////////////////////////////////////////
//   函数名称：tcp_server_start
//   功能：启动Tcp监听，开始接受客户端连接
//   参数：
//       sock:FLXInt32,服务器端套接字
//       port:unsigned FLXInt32,端口号
//       callback:tcp_server_acceptClientCallBack
//   返回值：
//   如果启动成功则返回0,否则返回-1
///////////////////////////////////////////////////////////////////////////

FLXInt32 tcp_server_start(FLXSocket sock, FLXUnint32 port, tcp_server_acceptClientCallBack callback)
{
	struct sockaddr_in my_addr;
	FLXUnint32 iMaxConnectNum = 1000;
	FLXUnint32 ret;
	FLXInt32 err, sock_reuse = 1;
	AcceptClientThreadParam *threadParams;
	
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	err = setsockopt(sock,SOL_SOCKET, SO_REUSEADDR, (FLXChar *)&sock_reuse, sizeof(sock_reuse)); 
	if(err != 0)
	{ 
		TCP_SERVER_LIB_DEBUG(("套接字可重用设置失败!\n"));
		return -1; 
	}

	if(bind(sock, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) 
	{
		TCP_SERVER_LIB_DEBUG(("Bind error\n"));
		return -1;
	}

	 if(listen(sock, iMaxConnectNum) == -1) 
	 {
		TCP_SERVER_LIB_DEBUG(("Listen error\n"));
		return -1;
	 }

     threadParams = (AcceptClientThreadParam *)malloc(sizeof(AcceptClientThreadParam));
     threadParams->serverSock  =sock;
	 threadParams->acceptClientCallBack = callback;

	 TCP_SERVER_LIB_DEBUG(("debug:threadParam.serverSock=%d\n", threadParams->serverSock));
	 
	 ret = thread_create((void *)acceptClientThread, (void *)threadParams);
	 if (ret != 0)
	 {
		TCP_SERVER_LIB_DEBUG(("thread create error\n"));
		return -1;
	 }

	 return 0;
}

//////////////////////////////////////////////////////////////////////////
//   函数名称：tcp_server_sendData
//   功能：发送数据给客户端sock
//   参数：
//        sock:FLXInt32，客户端套接字
//        data:char *，数据
//        len:FLXInt32，data的长度
//   返回值：
//       如果发送成功，则返回0,否则返回-1
/////////////////////////////////////////////////////////////////////////
//FLXInt32 tcp_server_sendData(CLIENT_INFO_STRU *clientInfo, FLXChar *data, FLXUnint32 dataLength)
FLXInt32 tcp_server_sendData(FLXSocket sock, const uint8_t *data, int dataLength)
{
	FLXUnint32 ret = 0, bytes_left = 0, first_byte = 0;
	
	if((sock <= 0) || (dataLength <= 0)) 
	{
		return -1;
	}
		
	bytes_left = dataLength;
	while(bytes_left > 0)
	{
#ifdef FLXWIN
		ret = send(sock, &data[first_byte], bytes_left, 0);
#elif defined(FLXLINUX) || defined(FLXUNIX)
		//ret = send(sock, &data[first_byte], bytes_left, MSG_NOSIGNAL);
		ret = send(sock, &data[first_byte], bytes_left, 0);
#endif
		if(ret <= 0) 
		{
			return -1; 
		}
			
		bytes_left -= ret;
		first_byte += ret;
	}
	return 0; 
}


int tcp_send_async(FLXSocket sock, const uint8_t *buffer, int data_len)
{
	int sended = 0;
	if(sock <= 0 || !buffer || data_len <= 0)
		return 0;

	while(data_len > 0)
	{
		int len = send(sock, &buffer[sended], data_len, 0);
		if(len <= 0)
		{
			if(len == 0)
			{
				return 0;
			}
			else{
				if (errno != EAGAIN)
					return 0;
			}
			continue;
		}


		data_len -= len;
		sended += len;
	}
	//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>  send = %d\n\n", sended);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
//   函数名称：tcp_server_readData
//   功能：从客户端套接字sock,读取数据
//   参数：
//        sock:FLXInt32，客户端套接字
//        data:char *，数据
//        len:FLXInt32，data的长度
//   返回值：
//       如果读取成功，则返回读取数据的长度,否则返回0，出错返回-1
/////////////////////////////////////////////////////////////////////////
FLXInt32 tcp_server_readData(FLXSocket sock, FLXChar *buffer, FLXUnint32 bufferSize)
{
	FLXUnint32 recvbytes;

	if ((sock <= 0) || (buffer == NULL) || (bufferSize <= 0)) 
	{
		return -1;
	}
	memset(buffer, 0, sizeof(buffer));
	recvbytes = recv(sock, buffer, bufferSize, 0);
	if (recvbytes == 0) 
	{
		TCP_SERVER_LIB_DEBUG(("Connection Closed.\n"));
		return 0;
	}
	else if (recvbytes == SOCKET_ERROR)
	{
		TCP_SERVER_LIB_DEBUG(("Socket Error.\n"));
		return -1;
	}
	TCP_SERVER_LIB_DEBUG(("recvbytes = %d\n", recvbytes));
	
	return recvbytes;
}

///////////////////////////////////////////////////////////////////////////
//   函数名称：tcp_server_stop
//   功能：停止Tcp监听，停止接受客户端连接
//   参数：
//       sock:FLXInt32,服务器端套接字
//   返回值：
//       如果成功，则返回0,否则返回-1
//////////////////////////////////////////////////////////////////////////
FLXInt32 tcp_server_stop(FLXSocket sock)
{
	if (INVALID_SOCKET == sock)
		return -1;

	return close(sock);
}


