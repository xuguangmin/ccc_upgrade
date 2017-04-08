/*****************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 *****************************************************************************
  �ļ����� : packet_pool.c
  ����    : ���Ӹ�
  �������� : 2013-3-27

  �汾    : 1.0
  �������� : ����Ļ�������ͷβ�ڵ�֮��������Ǳ����л����ָ��Ľڵ�
            ����ֱ�Ӵ�����ȡһ�������
            ���û��ȡ�������ֱ�Ӵ���һ���������Ȼ��ֱ�ӷ���

            ����ʹ�õĻ��������β�ڵ㴦���棬������������޽ڵ㣬��ᴴ��һ���ڵ�
            ʹ����Ҳ����ѡ��ֱ��ɾ��ʹ�ù��Ļ����

  �޸���ʷ :

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "ccc_packet.h"

typedef struct packet_node *PACKET_NODE;
struct packet_node
{
	CCCPACKET     *pkt_ptr;
	PACKET_NODE    next;
};

typedef struct packet_node_list
{
	PACKET_NODE    head;
	PACKET_NODE    tail;
	int            ring_size;                 /* ����������Ľڵ��� */
	int            pkt_count;                 /* ���������ݰ��ĸ��� */
	int            pkt_count_max;
}PACKET_NODE_LIST;


static PACKET_NODE_LIST  g_packet_list_idle  = {NULL, NULL, 0, 0, 128};
static pthread_mutex_t   g_packet_pool_mutex = PTHREAD_MUTEX_INITIALIZER;

static void packet_pool_lock()
{
	pthread_mutex_lock(&g_packet_pool_mutex);
}
static void packet_pool_unlock()
{
	pthread_mutex_unlock(&g_packet_pool_mutex);
}


static PACKET_NODE internal_packet_pool_node_create()
{
	PACKET_NODE lp_node = (PACKET_NODE)malloc(sizeof(struct packet_node));
	if(!lp_node)
		return NULL;

	lp_node->pkt_ptr = NULL;
	lp_node->next    = NULL;
	return lp_node;
}

static void internal_packet_pool_node_free(PACKET_NODE lp_node)
{
	free(lp_node);
}

/* �γɵ����������½ڵ���뵽tail֮�� */
static void internal_packet_pool_node_insert(PACKET_NODE_LIST *lp_db_list, PACKET_NODE lp_node)
{
	if(lp_db_list->ring_size <= 0)
	{
		lp_db_list->head = lp_node;
		lp_db_list->tail = lp_node;
		lp_node->next    = lp_node;
	}
	else
	{
		lp_node->next          = lp_db_list->tail->next;
		lp_db_list->tail->next = lp_node;
	}
	lp_db_list->ring_size++;
}

/* ��tail֮��ɾ */
static void internal_packet_pool_node_delete(PACKET_NODE_LIST *lp_pkt_list)
{
	PACKET_NODE node;
	if(!lp_pkt_list ||
		lp_pkt_list->ring_size <= 0)
		return;

	if(1 == lp_pkt_list->ring_size)
	{
		internal_packet_pool_node_free(lp_pkt_list->tail);

		lp_pkt_list->head = NULL;
		lp_pkt_list->tail = NULL;
		lp_pkt_list->ring_size = 0;
	}
	else
	{
		node                    = lp_pkt_list->tail->next;
		lp_pkt_list->tail->next = node->next;

		internal_packet_pool_node_free(node);
		lp_pkt_list->ring_size--;
	}
}

/* �����ڵ㣬������ */
static int internal_packet_pool_node_add(PACKET_NODE_LIST *lp_pkt_list)
{
	PACKET_NODE lp_node = internal_packet_pool_node_create();
	if(NULL == lp_node)
		return 0;

	internal_packet_pool_node_insert(lp_pkt_list, lp_node);
	return 1;
}

static int internal_packet_pool_save_from_tail(PACKET_NODE_LIST *lp_pkt_list, CCCPACKET *pkt_ptr)
{
	PACKET_NODE lp_node;
	if(!lp_pkt_list || !pkt_ptr)
		return 0;

	if(lp_pkt_list->ring_size == lp_pkt_list->pkt_count) /* �����������׷��һ�� */
	{
		if(!internal_packet_pool_node_add(lp_pkt_list))
			return 0;
	}

	lp_node           = lp_pkt_list->tail->next;
	lp_node->pkt_ptr  = pkt_ptr;

	lp_pkt_list->tail = lp_node;
	lp_pkt_list->pkt_count++;

	//printf("packet pool :node = %d, now packet = %d\n", lp_pkt_list->ring_size, lp_pkt_list->pkt_count);
	return 1;
}
static CCCPACKET *internal_packet_pool_get_from_head(PACKET_NODE_LIST *lp_db_list)
{
	CCCPACKET *result = NULL;
	if(!lp_db_list ||
		lp_db_list->ring_size <= 0 ||
		lp_db_list->pkt_count <= 0)
		return NULL;

	result = lp_db_list->head->pkt_ptr;

	lp_db_list->head->pkt_ptr = NULL;
	lp_db_list->head          = lp_db_list->head->next;
	lp_db_list->pkt_count--;

	return result;
}

/* ɾ��һЩ�� */
static void internal_packet_pool_delete(PACKET_NODE_LIST *lp_pkt_list)
{
	CCCPACKET *packet;
	while(lp_pkt_list->pkt_count > lp_pkt_list->pkt_count_max)
	{
		packet = internal_packet_pool_get_from_head(lp_pkt_list);
		if(packet) ccc_packet_delete(packet);
	}

	while(lp_pkt_list->ring_size > lp_pkt_list->pkt_count)
	{
		internal_packet_pool_node_delete(lp_pkt_list);
	}

	//printf("deleted. packet pool :node = %d, now packet = %d\n", lp_pkt_list->ring_size, lp_pkt_list->pkt_count);
}



static int internal_packet_pool_set(PACKET_NODE_LIST *lp_pkt_list, CCCPACKET *pkt_ptr)
{
	if(!internal_packet_pool_save_from_tail(lp_pkt_list, pkt_ptr))
		return 0;

	/* ��� */
	if(lp_pkt_list->pkt_count > lp_pkt_list->pkt_count_max) internal_packet_pool_delete(lp_pkt_list);
	return 1;
}
/*
 * ȡһ����
 * ���req_buffer_size > 0����ֱ�ӷ���ָ����С���ڴ�
 * ����������б�������ʱ���ŷ����ڴ�
 */
static CCCPACKET *internal_packet_pool_get(int req_buffer_size)
{
	CCCPACKET *result = internal_packet_pool_get_from_head(&g_packet_list_idle);
	if(!result)
	{
		/* �����½ڵ� */
		result = ccc_packet_create();
		//printf("%s ccc_packet_create\n", __FUNCTION__);
	}
	if(result && req_buffer_size > 0) ccc_packet_malloc(result, req_buffer_size);

	return result;
}


/* �Ѳ���ʹ�õİ������¹һ��б�β�� */
int packet_pool_set(CCCPACKET *pkt_ptr)
{
	int result;

	packet_pool_lock();
	result = internal_packet_pool_set(&g_packet_list_idle, pkt_ptr);
	packet_pool_unlock();
	return result;
}
/*
 * ȡһ������������Ϊbuffer
 * req_buffer_size >0 �������ڴ�
 */
CCCPACKET *packet_pool_get_buffer(int req_buffer_size)
{
	CCCPACKET *result;

	packet_pool_lock();
	result = internal_packet_pool_get(req_buffer_size);
	packet_pool_unlock();

	if(!result) printf("error !!!  get_packet_from_packet_pool NULL\n");
	return result;
}

/*
 * ȡһ������������ΪPDU
 * �����ڴ�
 * PDU�ĳ�Աdataָ��buffer
 */
CCCPACKET *packet_pool_get_pdu_buf(int buffer_size)
{
	CCCPACKET *result = packet_pool_get_buffer(buffer_size);
	if(result)
	{
		result->un.pdu.data = result->buffer;
	}
	return result;
}
/*
 * ȡһ����
 * ��ʱ���������ڴ�
 * ��������ʱ���ż���ڴ��Ƿ��㹻
 */
CCCPACKET *packet_pool_get()
{
	return packet_pool_get_buffer(0);
}

void packet_pool_size_max(int size)
{

}
