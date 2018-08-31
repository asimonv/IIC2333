#import "queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue* queue_init(){
  Queue* queue = malloc(sizeof(Queue));
  LinkedList* list = linkedlist_init();
  queue->list = list;
  return queue;
}

void queue_push(Queue* queue, node* item, bool at_tail) {
  linkedlist_insert(queue->list, item, at_tail);
}

void queue_poppush(Queue* queue) {
  node* node = linkedlist_delete(queue->list, true);
  linkedlist_insert(queue->list, node, false);
}

node* queue_pop(Queue* queue) {
  return linkedlist_delete(queue->list, false);
}
