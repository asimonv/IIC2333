#pragma once
#import "../arraylist/arraylist.h"

#define MAX_STRING_LEN 80

Process* process_init(ArrayList *timeline, char* name, int cpu_burst, int initial_time);

void process_print(Process *process);
