/* https://eddmann.com/posts/implementing-a-doubly-linked-list-in-c/ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#import "queue/queue.h"
#import "process/process.h"

int QUANTUM_TIME = 3;

int lines(char* filename) {
  char *line = NULL;
  FILE *file;
  size_t len = 0;
  ssize_t read;
  int res = 0;

  file = fopen(filename, "r");

  if (file) {
      while ((read = getline(&line, &len, file)) != -1) {
        res+=1;
      }
      free(line);
  }

  return res;
}

void generate_report(node* processes, char* output_filename, int process_count) {
  FILE *file = fopen(output_filename, "w+");
  if (file != NULL) {
    for (size_t i = 0; i < process_count; i++) {
      int turnaround = processes[i].item->end_time - processes[i].item->initial_time;
      int response_time = processes[i].item->end_time - processes[i].item->start_time;
      fprintf(file, "%s, %d, %d, %d, %d, %d\n", processes[i].item->name, processes[i].item->times_selected, processes[i].item->times_interrupted, turnaround, response_time, processes[i].item->waiting_time);
    }
  }
}

void print_change(char* state, int change_type, int current_time, node *process) {
  //state change
  if (change_type == 0) {
    printf("[t = %d] process %s has changed state to: %s\n", current_time, process->item->name, state);
  } else if (change_type == 1) {
    printf("[t = %d] process %s has been created\n", current_time, process->item->name);
  }
}

/* https://stackoverflow.com/a/13372759/5666746 */
int compare(const void *s1, const void *s2)
{
  node *e1 = (node*)s1;
  node *e2 = (node*)s2;
  printf("%s,%s\n", e1->item->name, e2->item->name);
  return e1->item->initial_time - e2->item->initial_time;
}

void check_push_process(node* processes, int process_count, int current_time, Queue* ready_queue) {
  for (size_t i = 0; i < process_count; i++) {
    if (processes[i].item->initial_time == current_time) {
      bool at_tail = !(ready_queue->list->head != NULL && strcmp(ready_queue->list->head->item->state, "WAITING") && ready_queue->list->head->item->timeline->array[ready_queue->list->head->item->curr_idx] == 1);
      processes[i].item->state = "READY";
      print_change(NULL, 1, current_time, &processes[i]);
      queue_push(ready_queue, &processes[i], at_tail);
    }
  }
}

void check_queue(Queue *ready_queue, int quantum, node* waiting_list, int* finished, int current_time) {
  if (ready_queue->list->head != NULL) {
    node *curr_process = ready_queue->list->head;
    ArrayList *curr_timeline = curr_process->item->timeline;

    // must interrupt
    if (curr_process->item->curr_quantum >= QUANTUM_TIME) {
      printf("quantum finished for %s\n", curr_process->item->name);
      curr_process->item->curr_quantum = 0;
      if (curr_timeline->array[curr_process->item->curr_idx] <= 0) {
        printf("curr_idx inc\n");
        curr_process->item->curr_idx += 1;
        //process_print(curr_process->item);
        queue_pop(ready_queue);

        //process has finished
        if (curr_process->item->curr_idx > curr_process->item->timeline->count){
          curr_process->item->state = "FINISHED";
          print_change(curr_process->item->state, 0, current_time, curr_process);
          curr_process->item->end_time = current_time;
          *finished -= 1;
        } else {
            curr_process->item->state = "WAITING";
            print_change(curr_process->item->state, 0, current_time, curr_process);
        }
      } else {
          queue_poppush(ready_queue);
          curr_process->item->times_interrupted += 1;
          //process goes to the end of queue in a READY state
          curr_process->item->state = "READY";
          print_change(curr_process->item->state, 0, current_time, curr_process);
        }

    } else {
      //process finished before end of quantum
      if (curr_timeline->array[curr_process->item->curr_idx] <= 0) {
        printf("curr_idx inc\n");
        curr_process->item->curr_idx += 1;
        queue_pop(ready_queue);

        //process has finished
        if (curr_process->item->curr_idx > curr_process->item->timeline->count){
          curr_process->item->state = "FINISHED";
          print_change(curr_process->item->state, 0, current_time, curr_process);
          curr_process->item->end_time = current_time;
          *finished -= 1;
        } else {
            curr_process->item->state = "WAITING";
            print_change(curr_process->item->state, 0, current_time, curr_process);
        }
      } else {
          /*queue_poppush(ready_queue);
          curr_process->item->times_interrupted += 1;
          //process goes to the end of queue in a READY state
          curr_process->item->state = "READY";
          print_change(curr_process->item->state, 0, current_time, curr_process);
          */
      }
    }
  } else {
    //printf("ready_queue is idle \n");
  }
}

void exec_quantum(Queue* ready_queue, int quantum, node* waiting_list, int *finished, int current_time) {
  if (ready_queue->list->head != NULL) {
    node* curr_process = ready_queue->list->head;
    curr_process->item->curr_quantum += 1;

    //process was selected to use CPU
    printf("%s quantum %d\n", curr_process->item->name, curr_process->item->curr_quantum);
    if (curr_process->item->curr_quantum == 1) {
      curr_process->item->times_selected += 1;
      printf("%s times_selected %d\n", curr_process->item->name, curr_process->item->times_selected);
      //process changes to RUNNING state
      curr_process->item->state = "RUNNING";
      print_change(curr_process->item->state, 0, current_time, curr_process);
    }

    // is first execution?
    if (curr_process->item->has_executed == 0) {
      curr_process->item->has_executed = 1;
      curr_process->item->start_time = current_time;
    }

    ArrayList *curr_timeline = curr_process->item->timeline;

    curr_timeline->array[curr_process->item->curr_idx] -= 1;
    printf("%s curr_burst = %d\n", curr_process->item->name, curr_timeline->array[curr_process->item->curr_idx]);
    arraylist_print(curr_process->item->timeline);
    /*
      current execution finished,
      process must go to waiting state
    */

  }
}

void tick_waiting(node* list, Queue *ready_queue, int current_time, int process_count, int* finished) {
  for (size_t i = 0; i < process_count; i++) {
    node* process = &list[i];
    int curr_idx = process->item->curr_idx;
    /*
      odd means that process it's in a WAITING state,
      must tick
    */
    if (curr_idx % 2 != 0) {
      if (strcmp(process->item->state, "WAITING") == 0) {
        //actual ticking
        printf("current_time: %d ", current_time);
        process_print(process->item);
        process->item->timeline->array[curr_idx] -= 1;
        //update waiting time: WAITING
        process->item->waiting_time += 1;
        //ticking finished -> push it to ready/finished
        if (process->item->timeline->array[curr_idx] <= 0) {
          process->item->curr_idx += 1;
          //it finished?
          if (process->item->curr_idx > process->item->timeline->count) {
            //process has finished
            arraylist_print(process->item->timeline);
            printf("index: %d, len: %d\n", process->item->curr_idx, process->item->timeline->count);
            process->item->state = "FINISHED";
            print_change(process->item->state, 0, current_time, process);
            *finished -= 1;
          } else {
            queue_push(ready_queue, process, false);
            process->item->state = "READY";
            print_change(process->item->state, 0, current_time, process);
          }
        }
        // else process needs to keep ticking
      }
    }
    process = process->next;
  }
}

int main(int argc, char *argv[]){

    if (argc < 2) {
      printf("modo de uso: ./scheduler <file> <output> [<quantum>]\n");
      exit(1);
    }

    Queue* ready_queue = queue_init();
    char *line = NULL;
  	FILE *file;
  	size_t len = 0;
  	ssize_t read;

  	file = fopen(argv[1], "r");
    int process_count = lines(argv[1]);
    node* processes = (node*) malloc(sizeof(node)*process_count);

    int current_time = 0;
    int finished = process_count;

    if (file) {
        int i = 0;
        while ((read = getline(&line, &len, file)) != -1) {
          //printf("%s", line);
          ArrayList* list = arraylist_init();
          char* token = strtok(line, " ");
          char* name = malloc(sizeof(token));
          strcpy(name, token);
          token = strtok(NULL, " ");

          int initial_time = atoi(token);
          token = strtok(NULL, " ");

          int cpu_burst = atoi(token);
          token = strtok(NULL, " ");

          while (token) {
            //printf("'%s'\n", token);
            arraylist_append(list, atoi(token));
		        token = strtok(NULL, " ");
          }
          Process* process = process_init(list, name, cpu_burst, initial_time);
          processes[i].item = process;
          i+=1;
        }
        free(line);
    }

    /*for (size_t i = 0; i < process_count; i++) {
      printf("%s, %d\n", processes[i].item->name, processes[i].item->initial_time);
    }*/

    qsort(processes, process_count, sizeof(processes[0]), compare);

    while (finished != 0) {
      //printf("current_time: %d\n", current_time);
      check_queue(ready_queue, argc == 4 ? atoi(argv[3]) : QUANTUM_TIME, processes, &finished, current_time);
      check_push_process(processes, process_count, current_time, ready_queue);
      tick_waiting(processes, ready_queue, current_time, process_count, &finished);
      //printf("queue: ");
      //linkedlist_print(ready_queue->list);
      exec_quantum(ready_queue, argc == 4 ? atoi(argv[3]) : QUANTUM_TIME, processes, &finished, current_time);
      current_time += 1;
    }

    generate_report(processes, argv[2], process_count);

    /*for (size_t i = 0; i < process_count; i++) {
      printf("%s, %d\n", processes[i].item->name, processes[i].item->initial_time);
    }*/
}
