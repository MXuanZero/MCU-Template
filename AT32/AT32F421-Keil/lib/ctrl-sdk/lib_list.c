/* Includes ------------------------------------------------------------------*/
#include "lib_list.h"
#include "lib_assert.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void lib_list_init(lib_list_t *list, size_t size)
{
	lib_assert(list == NULL);
	list->head = NULL;
	list->tail = NULL;
	size = (size + 3) & (~0x3);
	list->size = size;
}

void lib_list_inc_head(lib_list_t *list, lib_node_t *new_node)
{
	lib_assert(list == NULL);
	lib_assert(new_node == NULL);
	lib_node_t *head = list->head;
	new_node->prev = NULL;
	new_node->next = head;

	if (head != NULL) {
		head->prev = new_node;
	}
	list->head = new_node;
	if (list->tail == NULL) {
		list->tail = new_node;
	}
}

void lib_list_inc_tail(lib_list_t *list, lib_node_t *new_node)
{
	lib_assert(list == NULL);
	lib_assert(new_node == NULL);
	lib_node_t *tail = list->tail;

	new_node->prev = tail;
	new_node->next = NULL;

	if (tail != NULL) {
		tail->next = new_node;
	}
	list->tail = new_node;

	if (list->head == NULL) {
		list->head = new_node;
	}
}

void lib_list_inc_next(lib_list_t *list, lib_node_t *node, lib_node_t *new_node)
{
	lib_assert(list == NULL);
	lib_assert(node == NULL);
	lib_assert(new_node == NULL);

	lib_node_t *buf = NULL;
	buf = node->next;
	node->next = new_node;
	new_node->next = buf;
	if (buf != NULL) {
		buf->prev = new_node;
	} else {
		list->tail = new_node;
	}
	new_node->prev = node;
}

void lib_list_inc_prev(lib_list_t *list, lib_node_t *node, lib_node_t *new_node)
{
	lib_assert(list == NULL);
	lib_assert(node == NULL);
	lib_assert(new_node == NULL);

	lib_node_t *buf = node->prev;
	node->prev = new_node;
	new_node->prev = buf;
	if (buf != NULL) {
		buf->next = new_node;
	} else {
		list->head = new_node;
	}
	new_node->next = node;
}

void lib_list_delete(lib_list_t *list, lib_node_t *node)
{
	lib_assert(list == NULL);
	lib_assert(node == NULL);

	if (node->next == NULL && node->prev == NULL) {
		list->head = NULL;
		list->tail = NULL;
	} else if (node->next == NULL) {
		node->prev->next = NULL;
		list->tail = node->prev;
		node->prev = NULL;
	} else if (node->prev == NULL) {
		node->next->prev = NULL;
		list->head = node->next;
		node->next = NULL;
	} else {
		node->next->prev = node->prev;
		node->prev->next = node->next;
		node->next = NULL;
		node->prev = NULL;
	}
}

void lib_list_change_node(lib_list_t *olist, lib_list_t *nlist,
			  lib_node_t *node, bool head)
{
	lib_assert(olist == NULL);
	lib_assert(nlist == NULL);
	lib_assert(nlist == NULL);

	lib_list_delete(olist, node); // 断开节点连接
	if (head) {
		lib_node_t *head_node = nlist->head;
		node->prev = NULL;
		node->next = head_node;
		if (head_node != NULL) {
			head_node->prev = node;
		}
		nlist->head = node;
		if (nlist->tail == NULL) {
			nlist->tail = node;
		}
	} else {
		lib_node_t *tail_node = nlist->tail;
		node->prev = tail_node;
		node->next = NULL;
		if (tail_node != NULL) {
			tail_node->next = node;
		}
		nlist->tail = node;
		if (nlist->head == NULL) {
			nlist->head = node;
		}
	}
}

void lib_list_move_node(lib_list_t *list, lib_node_t *act, lib_node_t *node,
			bool before)
{
	lib_assert(list == NULL);
	lib_assert(act == NULL);
	lib_assert(node == NULL);

	lib_list_delete(list, act); // 断开节点连接

	/* 连接新的节点 */
	if (node->next == NULL && node->prev == NULL) {
		return; // 出现这种情况即为出错
	}
	if (before) {
		if (node->prev == NULL) {
			act->next = node;
			node->prev = act;
			list->head = act;
		} else {
			node->prev->next = act;
			act->prev = node->prev;
			node->prev = act;
			act->next = node;
		}
	} else {
		if (node->next == NULL) {
			act->prev = node;
			node->next = act;
			list->head = act;
		} else {
			node->next->prev = act;
			act->next = node->next;
			node->next = act;
			act->prev = node;
		}
	}
}

size_t lib_list_get_len(const lib_list_t *list)
{
	if (list == NULL) {
		return 0;
	}
	lib_node_t *node = list->head;
	size_t len = 0;
	while (node != NULL) {
		node = node->next;
		++len;
	}
	return len;
}

inline bool lib_list_is_empty(lib_list_t *list)
{
	if (list == NULL) {
		return false;
	}
	if (list->head == NULL && list->tail == NULL) {
		return true;
	}
	return false;
}
