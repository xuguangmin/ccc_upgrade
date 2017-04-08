#ifndef TCP_SERVER_HEAHER_FILE
#define TCP_SERVER_HEAHER_FILE
////////////////////////////////////////////////////////////////////////////////////
//�ļ����ƣ�tcpserver.h
//�ļ�˵����
//			 Tcp������������ӿں���ͷ�ļ�
//�汾��
//			v 1.0
//����ʱ��:
//			2010��9��13��
//�����ˣ�
//			 ����
//�޸��ˣ�
//			��־��(����ʵ��д�Ա��ά��)
//�޸�ʱ�䣺
//			2010��11��23��(����ʵ��д�Ա��ά��)
//�޸����ݣ�
//			���windowƽ̨��socket�������Ż�
///////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus 
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "flxtypes.h"
#include "flxnettypes.h"

#ifdef FLXWIN
#include <windows.h>
#include <winbase.h>
#include <stddef.h>
#include <string.h>

#elif defined(FLXLINUX) || defined(FLXUNIX)
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif



#ifdef DEBUG
#define TCP_SERVER_LIB_DEBUG(args)	(printf("[tcp_server_lib:]"), printf args)
#else
#define TCP_SERVER_LIB_DEBUG(args)
#endif

//////////////////////////////////////////////////////////////////////////
//   �������ƣ�tcp_server_acceptClientCallBack
//   ���ܣ����ܿͻ������ӻص�����
//   ������
//        serverSock:FlxSocket���ͻ����׽���
//        endpoint:FLXEndPoint *���ͻ���
//   ����ֵ��
//       �ɹ����򷵻�0,���򷵻�-1
/////////////////////////////////////////////////////////////////////////
typedef FLXInt32 (*tcp_server_acceptClientCallBack)(FLXSocket sock, const char *ipAddr, int port);

////////////////////////////////////////////////////////////////////////////
//   ��������:tcp_server_initialize
//   ���ܣ���ʼ��Tcp�����������
//   ��������
//   ����ֵ��
//       �����ʼ���ɹ����򷵻��ѳ�ʼ�����׽���,���򷵻�-1
///////////////////////////////////////////////////////////////////////////
FLXSocket tcp_server_initialize(void);

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
FLXInt32 tcp_server_start(FLXSocket sock, FLXUnint32 port, tcp_server_acceptClientCallBack callback);

//////////////////////////////////////////////////////////////////////////
//   �������ƣ�tcp_server_sendData
//   ���ܣ��������ݸ��ͻ���sock
//   ������
//        sock:FLXInt32���ͻ����׽���
//        data:FLXChar *������
//        len:FLXInt32��data�ĳ���
//   ����ֵ��
//       ������ͳɹ����򷵻�0,���򷵻�-1
/////////////////////////////////////////////////////////////////////////
FLXInt32 tcp_server_sendData(FLXSocket sock, const uint8_t *data, int dataLength);

extern int tcp_send_async(FLXSocket sock, const uint8_t *buffer, int data_len);

//////////////////////////////////////////////////////////////////////////
//   �������ƣ�tcp_server_readData
//   ���ܣ��ӿͻ����׽���sock,��ȡ����
//   ������
//        sock:FLXInt32���ͻ����׽���
//        data:FLXChar *������
//        len:FLXInt32��data�ĳ���
//   ����ֵ��
//       �����ȡ�ɹ����򷵻ض�ȡ���ݵĳ���,���򷵻�0��������-1
/////////////////////////////////////////////////////////////////////////
FLXInt32 tcp_server_readData(FLXSocket sock, FLXChar *buffer, FLXUnint32 bufferSize);

///////////////////////////////////////////////////////////////////////////
//   �������ƣ�tcp_server_stop
//   ���ܣ�ֹͣTcp������ֹͣ���ܿͻ�������
//   ������
//       sock:FLXInt32,���������׽���
//   ����ֵ��
//       ����ɹ����򷵻�0,���򷵻�-1
//////////////////////////////////////////////////////////////////////////
FLXInt32 tcp_server_stop(FLXSocket sock);
FLXInt32 tcp_server_slow_stop(FLXSocket sock);


#ifdef __cplusplus 
}
#endif

#endif //end of ifndef TCP_SERVER_HEAHER_FILE
