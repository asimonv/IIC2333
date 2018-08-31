#pragma once

#include "../linkedlist/linkedlist.h"

typedef struct queue
{
  // root is always an empty node.
  LinkedList *list;
} Queue;

Queue* queue_init();

void queue_push(Queue* queue, node* item, bool at_tail);

void queue_poppush(Queue* queue);

node* queue_pop(Queue *queue);
