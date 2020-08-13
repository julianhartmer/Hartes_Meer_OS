#include "process.h"
#include <stdint.h>

#define ADDR_TO_MB(X)	((uint32_t) (X) >> 20)
#define PROCESS_SIZE_MB ADDR_TO_MB (&_user_rw_end - &_user_rw_start)
#define PROCESS_START_MB (ADDR_TO_MB (&_user_rw_end)+1)

extern const char _kernel_ro_start, _kernel_ro_end, _kernel_rw_start, _kernel_rw_end, _user_ro_start,
       _user_ro_end, _user_rw_start, _user_rw_end;

uint32_t processes[PROCESS_AMOUNT];
uint32_t p_usage[PROCESS_AMOUNT];

void init_processes() {
	for (int i = 0; i < PROCESS_AMOUNT; i++) {
		processes[i] = PROCESS_START_MB+PROCESS_SIZE_MB*i;
	}
}

// return process id
uint32_t get_free_process() {
	for (int i = 0; i < PROCESS_AMOUNT; i++) {
		if (p_usage[i] == 0) {
			p_usage[i]++;
			return i;
		}
	}
	return -1;
}

// return process id
void free_process(int i) {
	p_usage[i]--;
}

uint32_t get_process_mb(int i) {
	return processes[i];
}

void add_thread_to_process(int p_id) {
	p_usage[p_id]++;
}
