// Importo el archivo arraylist.h
#include "arraylist.h"
// Libreria estandar de C
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
//                             Funciones                                //
//////////////////////////////////////////////////////////////////////////

// TODO: Debes completar las siguientes funciones
// Puedes crear otras funciones aca para el
// funcionamiento interno del arreglo dinamico

int size=8;

/** Crea una arraylist inicialmente vacia y retorna su puntero */
ArrayList* arraylist_init()
{
  ArrayList* list = malloc(sizeof(ArrayList));
  list->array = malloc(sizeof(int)*size);
  list->count = 0;
  list->size = size;
  return list;
}

void arraylist_print(ArrayList *list) {
  printf("[");
  for (size_t i = 0; i < list->count; i++) {
    printf("%i ", list->array[i]);
  }
  printf("]\n");
}

void arraylist_expand(ArrayList *list) {
  //printf("expanding\n");
  list->size *= 2;
  list->array = realloc(list->array, sizeof(int)*list->size);

  /*
  //  copy all elements
  int *temp = malloc(sizeof(int)*list->size);
  for (size_t i = 0; i < list->count; i++) {
    temp[i] = list->array[i];
  }

  printf("expanding\n");
  for (size_t i = 0; i < list->count; i++) {
    printf("-> %i\n", list->array[i]);
  }

  list->array = temp;*/
}

/** Inserta un elemento al final de la arraylist */
void arraylist_append(ArrayList* list, int element)
{
  //printf("adding: %i\n", element);
  if (list->count == list->size) {
    arraylist_expand(list);
  }

  list->array[list->count] = element;
  list->count += 1;

  //arraylist_print(list);
}

/** Inserta el elemento dado en la posicion indicada */
void arraylist_insert(ArrayList* list, int element, int position)
{
  //printf("inserting: %i to %i\n", element, position);
  if (list->count == list->size) {
    arraylist_expand(list);
  }

  for (size_t i = list->count; i > position; i--) {
    list->array[i] = list->array[i-1];
  }

  list->array[position] = element;
  list->count += 1;

  //arraylist_print(list);

}

/** Elimina el elemento de la posicion indicada y lo retorna */
int arraylist_delete(ArrayList* list, int position)
{
  //printf("deleting %i\n", position);
  int elem = list->array[position];
  for (size_t i = position; i < list->count - 1; i++) {
    /* code */
    list->array[i] = list->array[i+1];
  }
  list->count -= 1;

  //arraylist_print(list);

  return elem;
}

/** Retorna el valor del elemento en la posicion dada */
int arraylist_get(ArrayList* list, int position)
{

  position = position >= 0
    ? (position%list->count)
    : list->count + position%list->count;

  return list->array[position];
}

/** Concatena la segunda arraylist a la primera arraylist */
void arraylist_concatenate(ArrayList* list1, ArrayList* list2)
{
  if (list1->count + list2->count > list1->size) {
    arraylist_expand(list1);
    arraylist_concatenate(list1, list2);
  } else {
    for (size_t i = 0; i < list2->count; i++) {
      list1->array[list1->count + i] = list2->array[i];
    }
    list1->count += list2->count;
    arraylist_destroy(list2);
    //arraylist_print(list1);
  }

}

/** Libera todos los recursos asociados a la lista */
void arraylist_destroy(ArrayList* list)
{
  free(list->array);
  free(list);
}
