#include <stdbool.h>
#import "../process/process.h"
// Esta linea sirve para que el codigo de este archivo solo se importe una vez
#pragma once

/** Estructura de una lista ligada */
typedef struct node {
    Process* item;
    struct node *prev, *next;
} node;

typedef struct linked_list {
    node* tail;
    node* head;
    int length;
} LinkedList;

//////////////////////////////////////////////////////////////////////////
//                             Funciones                                //
//////////////////////////////////////////////////////////////////////////

//OJO: No se debe modificar nada de esto

/** Crea una lista inicialmente vacia y retorna el puntero */
LinkedList* linkedlist_init();

void linkedlist_insert(LinkedList* list, node* item, bool at_tail);

node* linkedlist_delete(LinkedList* list, bool from_tail);

void linkedlist_remove(LinkedList* list, int position);

void linkedlist_print(LinkedList *list);

void linkedlist_destroy(LinkedList* list);
