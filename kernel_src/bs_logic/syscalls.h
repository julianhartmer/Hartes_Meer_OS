#include <stddef.h>

// returns 1 on success, negatives on error
int sys_create_thread(void *func, void * data, size_t length, int new_process);

int sys_kill_thread();

int sys_sleep_thread();

int sys_send_char(char c);

char sys_read_char();
