// Esta linea sirve para que el código de este archivo solo se importe una vez
#pragma once


/** Estructura de una arraylist */
struct arraylist_list
{
  int size;
  int count;
  int *array;
};

// Aquí le estoy poniendo un nombre más simple a la lista para no tener que
// referirme a la lista como struct arraylist_list
/** Estructura de una arraylist */
typedef struct arraylist_list ArrayList;

typedef struct process {
  ArrayList *timeline;
  char* name;
  int curr_quantum;
  int cpu_burst;
  int initial_time;
  int end_time;
  int start_time;
  int waiting_time;
  int curr_idx;
  int has_executed;
  int times_selected;
  int times_interrupted;
  char* state;
} Process;


//////////////////////////////////////////////////////////////////////////
//                             Funciones                                //
//////////////////////////////////////////////////////////////////////////

//OJO: No se debe modificar nada de esto

/** Crea una arraylist inicialmente vacia y retorna su puntero */
ArrayList* arraylist_init();

/** Inserta un elemento al final de la arraylist */
void arraylist_append(ArrayList* list, int element);

/** Inserta el elemento dado en la posicion indicada */
void arraylist_insert(ArrayList* list, int element, int position);

/** Elimina el elemento de la posicion indicada y lo retorna */
int arraylist_delete(ArrayList* list, int position);

/** Retorna el valor del elemento en la posicion dada */
int arraylist_get(ArrayList* list, int position);

/** Concatena la segunda arraylist a la primera arraylist */
void arraylist_concatenate(ArrayList* list1, ArrayList* list2);

/** Libera todos los recursos asociados a la lista */
void arraylist_destroy(ArrayList* list);

void arraylist_print(ArrayList * list);
