#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

#define PROCESS_AMOUNT 8

void init_processes();

// return process id
uint32_t get_free_process();

void free_process(int i);

uint32_t get_process_mb(int i);

void add_thread_to_process(int p_id);
#endif
