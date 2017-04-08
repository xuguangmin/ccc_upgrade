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
//   �������ƣ�tcp_server_acceptClientCallBack
//   ���ܣ����ܿͻ������ӻص�����
//   ������
//        serverSock:FlxSocket���ͻ����׽���
//        endpoint:FLXEndPoint *���ͻ���
//   ����ֵ��
//       ������ͳɹ����򷵻�0,���򷵻�-1
/////////////////////////////////////////////////////////////////////////
//typedef FLXInt32 (*tcp_server_acceptClientCallBack)(FLXSocket *sock,FLXIpEndPoint* ipEndpoint);

////////////////////////////////////////////////////////////////////////////////////
//���ܿͻ����̲߳�������
////////////////////////////////////////////////////////////////////////////////////
typedef struct acceptClientThreadParam
{
     FLXSocket serverSock;
     void * acceptClientCallBack;//FLXInt32 (*tcp_server_acceptClientCallBack)(FLXSocket *sock,FLXIpEndPoint* ipEndpoint);���ܵ��ͻ���
}AcceptClientThreadParam, *PAcceptClientThreadParam;

////////////////////////////////////////////////////////////////////////////
//   ��������:tcp_server_initialize
//   ���ܣ���ʼ��Tcp�����������
//   ��������
//   ����ֵ��
//       �����ʼ���ɹ����򷵻��ѳ�ʼ�����׽���,���򷵻�-1
///////////////////////////////////////////////////////////////////////////
FLXSocket tcp_server_initialize(void)
{
		FLXSocket iSocket;
	
#ifdef FLXWIN
		WSADATA wsaData;
		FLXInt32 iResult = WSAStartup(MAKEWORD(2,2), &wsaData); 	//��ʼ����WS2_32.DLL
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
	struct sockaddr_in remote_addr;		/* �ͻ��˵�ַ��Ϣ */
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
 		 if(callback(client_fd, ipAddress, ipPort) != 0)   /* ������ز��ɹ�ֵ���رո�sock*/
 			 close(client_fd);
	}
	return ((void*)0);
}

////////////////////////////////////////////////////////////////////////////
//   �������ƣ�tcp_server_start
//   ���ܣ�����Tcp��������ʼ���ܿͻ�������
//   ������
//       sock:FLXInt32,���������׽���
//       port:unsigned FLXInt32,�˿ں�
//       callback:tcp_server_acceptClientCallBack
//   ����ֵ��
//   ��������ɹ��򷵻�0,���򷵻�-1
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
		TCP_SERVER_LIB_DEBUG(("�׽��ֿ���������ʧ��!\n"));
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
//   �������ƣ�tcp_server_sendData
//   ���ܣ��������ݸ��ͻ���sock
//   ������
//        sock:FLXInt32���ͻ����׽���
//        data:char *������
//        len:FLXInt32��data�ĳ���
//   ����ֵ��
//       ������ͳɹ����򷵻�0,���򷵻�-1
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
//   �������ƣ�tcp_server_readData
//   ���ܣ��ӿͻ����׽���sock,��ȡ����
//   ������
//        sock:FLXInt32���ͻ����׽���
//        data:char *������
//        len:FLXInt32��data�ĳ���
//   ����ֵ��
//       �����ȡ�ɹ����򷵻ض�ȡ���ݵĳ���,���򷵻�0��������-1
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
//   �������ƣ�tcp_server_stop
//   ���ܣ�ֹͣTcp������ֹͣ���ܿͻ�������
//   ������
//       sock:FLXInt32,���������׽���
//   ����ֵ��
//       ����ɹ����򷵻�0,���򷵻�-1
//////////////////////////////////////////////////////////////////////////
FLXInt32 tcp_server_stop(FLXSocket sock)
{
	if (INVALID_SOCKET == sock)
		return -1;

	return close(sock);
}


