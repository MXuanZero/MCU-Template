#ifndef LIB_LIST_H
#define LIB_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ctrl_defs.h"
/* Define --------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/**
 * @brief Gets the address offset of the variable in the structure.
 */
#ifndef addr_offset
#define addr_offset(type, member) ((size_t) & ((type *)0)->member)
#endif

/**
 * @brief Gets the starting address of the structure
 * @param i item
 * @param t type
 * @param m member
 */
#ifndef container_addr
#define container_addr(i, t, m) ((t *)((char *)(i)-addr_offset(t, m)))
#endif

/**
 * @brief 返回表头
 */
#define lib_list_get_head(l) ((l) != NULL ? (l)->head : NULL)

/**
 * @brief 返回表尾
 */
#define lib_list_get_tail(l) ((l) != NULL ? (l)->tail : NULL)

/**
 * @brief 顺序遍历链表
 * @param l 链表
 * @param n 节点
 */
#define lib_list_for_each(l, n) \
	for ((n) = lib_list_get_head(l); (n) != NULL; (n) = (n)->next)

/**
 * @brief 顺序安全遍历链表，如果遍历过程中需要删除链表可使用该宏
 * @param l 链表
 * @param n 节点
 * @param b 缓存节点
 */
#define lib_list_for_each_safe(l, n, t)              \
	for ((n) = lib_list_get_head((l)),           \
	    (t) = ((n) != NULL ? (n)->next : NULL); \
	     (n) != NULL; (n) = (t), (t) = ((n) != NULL ? (n)->next : NULL))

/**
 * @brief 逆序遍历链表
 * @param l 链表
 * @param n 节点
 */
#define lib_list_for_each_prev(l, n) \
	for ((n) = lib_list_get_tail(l); (n) != NULL; (n) = (n)->prev)

/**
 * @brief 逆序安全遍历链表，如果遍历过程中需要删除链表可使用该宏
 * @param l 链表
 * @param n 节点
 * @param b 缓存节点
 */
#define lib_list_for_each_prev_safe(l, n, t)         \
	for ((n) = lib_list_get_tail((l)),           \
	    (t) = ((n) != NULL ? (n)->next : NULL); \
	     (n) != NULL; (n) = (t), (t) = ((n) != NULL ? (n)->next : NULL))

/* Exported types ------------------------------------------------------------*/
struct lib_note;
struct lib_list;

typedef struct lib_note {
	struct lib_note *next;
	struct lib_note *prev;
} lib_node_t;

typedef struct lib_list {
	struct lib_note *head;
	struct lib_note *tail;
	size_t size;
} lib_list_t;

/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief 初始化链表
 */
void lib_list_init(lib_list_t *list, size_t size);

/**
 * @brief 添加新表头，返回新表头的地址
 */
void lib_list_inc_head(lib_list_t *list, lib_node_t *new_node);

/**
 * @brief 添加新表尾，返回表尾地址
 */
void lib_list_inc_tail(lib_list_t *list, lib_node_t *new_node);

/**
 * @brief 在指定链表中指定节点后增加新节点
 */
void lib_list_inc_next(lib_list_t *list, lib_node_t *node,
		       lib_node_t *new_node);

/**
 * @brief 在指定链表中指定节点前增加新节点
 */
void lib_list_inc_prev(lib_list_t *list, lib_node_t *node, lib_node_t *new_node);

/**
 * @brief 删除该节点
 */
void lib_list_delete(lib_list_t *list, lib_node_t *node);

/**
 * @brief 将一个节点移动到新链表
 * @param olist 节点所在的链表
 * @param nlist 新的链表
 * @param node  节点
 * @param head true: be the head in the nlist list
 *             false be the tail in the nlist list
 */
void lib_list_change_node(lib_list_t *olist, lib_list_t *nlist,
			  lib_node_t *node,
			 bool head);

/**
 * @brief 将一个节点移动到同一个链表中的另一个节点之前
 * @param n_act 要移动的节点
 * @param node_addr 目标位置
 * @param before 移动到该节点前
 */
void lib_list_move_node(lib_list_t *list, lib_node_t *act, lib_node_t *node,
		       bool before);

/**
 * @brief 返回整个链表的长度
 */
size_t lib_list_get_len(const lib_list_t *list);

/**
 * @brief 检查链表是否为空
 */
bool lib_list_is_empty(lib_list_t *list);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // LIB_LIST_H
