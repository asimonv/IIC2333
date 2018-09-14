// importo el archivo linkedlist.h
#include "linkedlist.h"
// Libreria estandar de C
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
//                             Funciones                                //
//////////////////////////////////////////////////////////////////////////

LinkedList* linkedlist_init() {
  LinkedList* list = malloc(sizeof(LinkedList));
  list->head = NULL;
  list->tail = NULL;
  list->length = 0;
  return list;
}

void linkedlist_insert(LinkedList* list, node* item, bool at_tail)
{
    item->prev = item->next = NULL;

    if (NULL == list->head) {
        list->head = list->tail = item;
    } else if (at_tail) {
        list->tail->next = item;
        item->prev = list->tail;
        list->tail = item;
    } else {
        item->next = list->head;
        list->head->prev = item;
        list->head = item;
    }
    list->length += 1;
}

void linkedlist_remove(LinkedList* list, int position) {
  if (position == 0) {
    linkedlist_delete(list, false);
  } else if (position == list->length) {
    linkedlist_delete(list, true);
  } else {
    node *curr = list->head;
    for (size_t i = 0; i < position - 1; i++) {
      curr = curr->next;
    }

    node *temp = curr->next;
    curr->next = temp->next;
    if (temp->next != NULL) {
      temp->next->prev = curr;
    }
    list->length -= 1;
  }
}

node* linkedlist_delete(LinkedList* list, bool from_tail)
{
    if (NULL == list->head) {
        printf("Empty list.\n");
        exit(1);
    } else if (from_tail) {
        node *ptr = list->tail;
        node* item = ptr;
        list->tail = ptr->prev;
        if (NULL == list->tail) list->head = list->tail;
        else list->tail->next = NULL;
        //free(ptr);
        ptr = NULL;
        list->length -= 1;
        return item;
    } else {
        node *ptr = list->head;
        node* item = ptr;
        list->head = ptr->next;
        if (NULL == list->head) list->tail = list->head;
        else list->head->prev = NULL;
        //free(ptr);
        ptr = NULL;
        list->length -= 1;
        return item;
    }
}

void linkedlist_print(LinkedList *list)
{
    node *ptr = list->head;

    while (NULL != ptr) {
        //printf("[%s %d]", ptr->item->name, ptr->item->timeline->array[ptr->item->curr_idx]);
        ptr = ptr->next;
    }

    printf("\n");
}

/** Libera todos los recursos asociados a la lista */
void linkedlist_destroy(LinkedList* list)
{
  node * it = list->head;
  while( NULL != it ) {
    node * tmp = it;
    it = it->next;
    free(tmp);
  }

  free(list);
}
