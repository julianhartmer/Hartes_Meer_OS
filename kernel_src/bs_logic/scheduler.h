#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stddef.h>
#include "bs_logic/context.h"

void schedule(void * context);
void scheduler_wakeup();
void scheduler_sleep();
int scheduler_kill_thread(void * context);
int scheduler_create_thread(struct thread_context *ctx);
void scheduler_thread_sleep(struct thread_context *ctx);
void scheduler_thread_wait(struct thread_context *ctx);
void scheduler_init(struct thread_context *ctx);
#endif
