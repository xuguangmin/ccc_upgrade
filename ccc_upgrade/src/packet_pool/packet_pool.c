/*****************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 *****************************************************************************
  文件名称 : packet_pool.c
  作者    : 贾延刚
  生成日期 : 2013-3-27

  版本    : 1.0
  功能描述 : 单向的环形链表，头尾节点之间的区域，是保存有缓存包指针的节点
            可以直接从其中取一个缓存包
            如果没有取到，则会直接创建一个缓存包，然后直接返回

            不再使用的缓存包，从尾节点处保存，如果环形链表无节点，则会创建一个节点
            使用者也可以选择直接删除使用过的缓存包

  修改历史 :

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
	int            ring_size;                 /* 单向环形链表的节点数 */
	int            pkt_count;                 /* 链表中数据包的个数 */
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

/* 形成单向环形链表，新节点插入到tail之后 */
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

/* 从tail之后删 */
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

/* 创建节点，并插入 */
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

	if(lp_pkt_list->ring_size == lp_pkt_list->pkt_count) /* 如果已满，新追加一个 */
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

/* 删除一些包 */
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

	/* 检查 */
	if(lp_pkt_list->pkt_count > lp_pkt_list->pkt_count_max) internal_packet_pool_delete(lp_pkt_list);
	return 1;
}
/*
 * 取一个包
 * 如果req_buffer_size > 0，则直接分配指定大小的内存
 * 否则，在向包中保存数据时，才分配内存
 */
static CCCPACKET *internal_packet_pool_get(int req_buffer_size)
{
	CCCPACKET *result = internal_packet_pool_get_from_head(&g_packet_list_idle);
	if(!result)
	{
		/* 创建新节点 */
		result = ccc_packet_create();
		//printf("%s ccc_packet_create\n", __FUNCTION__);
	}
	if(result && req_buffer_size > 0) ccc_packet_malloc(result, req_buffer_size);

	return result;
}


/* 把不再使用的包，重新挂回列表尾部 */
int packet_pool_set(CCCPACKET *pkt_ptr)
{
	int result;

	packet_pool_lock();
	result = internal_packet_pool_set(&g_packet_list_idle, pkt_ptr);
	packet_pool_unlock();
	return result;
}
/*
 * 取一个包，用来做为buffer
 * req_buffer_size >0 将分配内存
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
 * 取一个包，用来做为PDU
 * 分配内存
 * PDU的成员data指向buffer
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
 * 取一个包
 * 这时并不分配内存
 * 保存数据时，才检查内存是否足够
 */
CCCPACKET *packet_pool_get()
{
	return packet_pool_get_buffer(0);
}

void packet_pool_size_max(int size)
{

}
