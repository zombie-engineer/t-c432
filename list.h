#pragma once
#include <stdbool.h>

struct list_node {
  struct list_node *next;
  struct list_node *prev;
};

static inline void list_init(struct list_node *l)
{
  l->next = l->prev = l;
}

static inline bool list_is_empty(struct list_node *l)
{
  return l->next == l && l->prev == l;
}

/* Add list node 'node' to the end of the list 'list' */
static inline void list_add_tail(struct list_node *list, struct list_node *node)
{
  node->prev = list->prev;
  node->next = list;
  list->prev->next = node;
  list->prev = node;
}

/* Remove node from the end of the list */
static struct list_node *list_remove_head(struct list_node *list)
{
  struct list_node *node = list->next;

  list->next = list->next->next;
  list->next->prev = list;
  node->next = node->prev = node;
  return node;
}

static struct list_node *list_get_first(struct list_node *list)
{
  return list->next;
}

static inline void list_del(struct list_node *node)
{
  struct list_node *prev = node->prev;
  node->prev->next = node->next;
  node->next->prev = prev;
  node->next = node;
  node->prev = node;
}
