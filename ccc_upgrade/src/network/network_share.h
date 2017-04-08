/*
 * network_share.h
 *
 *  Created on: 2013��12��30��
 *      Author: flx
 */

#ifndef __NETWORK_SHARE_H__
#define __NETWORK_SHARE_H__

/*
 * ������
 *     handle   ���ͻ��˵ľ��
 *     buffer   ������
 *     data_len �����������ݳ���
 *     b_login  ���ͻ����Ƿ��Ѿ�����֤
 *
 * ����ֵ��
 *      buffer���Ѿ�ʹ�õ����ݵ����������ֽ�
 *
 */
typedef int (*network_manager_callback)(const void *handle, const uint8_t *buffer, int data_len, int b_login);

#endif /* __NETWORK_SHARE_H__ */
