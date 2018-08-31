#include "process.h"
// Libreria estandar de C
#include <stdlib.h>
#include <stdio.h>

Process* process_init(ArrayList *timeline, char* name, int cpu_burst, int initial_time) {
  Process* process = malloc(sizeof(Process));
  process->cpu_burst = cpu_burst;
  process->initial_time = initial_time;
  process->name = name;
  process->timeline = timeline;
  process->state = "READY";
  process->curr_idx = 0;
  process->end_time = 0;
  process->start_time = 0;
  process->has_executed = 0;
  process->times_selected = 0;
  process->times_interrupted = 0;
  process->waiting_time = 0;
  process->curr_quantum = 0;
  return process;
}

void process_print(Process *process) {
  printf("%s %d %d curr_idx: %d ", process->name, process->initial_time, process->cpu_burst, process->curr_idx);
  arraylist_print(process->timeline);
}
