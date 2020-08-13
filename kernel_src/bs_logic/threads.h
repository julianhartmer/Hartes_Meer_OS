#ifndef THREAD_H
#define THREAD_H
#include <stddef.h>

int new_thread(void *func, void *data, size_t len, int new_process);

void init_threads();

void thread_finish();

void thread_kill();

int amount_ready_threads();

int amount_free_threads();

int amount_sleeping_threads();

int amount_waiting_threads();

void safe_thread_context(void * ctx_p);

void next_thread();

void load_context(void * ctx_pointer);

void load_idle_thread(volatile void *ctx_pointer);

void print_context(void *ctx_pointer);

int is_idling();

void poke_sleeping_threads();

int feed_char_to_thread(char c);

void thread_sleep(int sleep_amount);

void thread_wait(void);
#endif
