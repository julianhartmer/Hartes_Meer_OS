#include "scheduler.h"

#include "bs_logic/threads.h"
#include "bs_logic/k_io.h"
#include "driver/arm_timer.h"

#define LIST_DMP() OS_PRINT ("\tamount of ready threads: %i\n\tamount sleeping threads: %i\n\tamount waiting threads: %i\n", amount_ready_threads(), amount_sleeping_threads(), amount_waiting_threads())
void schedule(void * context) {

	poke_sleeping_threads();
	OS_PRINT ("\n\t\tFREDDULER:\n");
	LIST_DMP ();
	if (NULL != context) {
		OS_PRINT ("\tcurrent context: \n");
	}
	if (amount_ready_threads() > 0) {
		if (!is_idling()) {
			OS_PRINT ("context nicht gespeicher!\n");
			safe_thread_context(context);
		} else if (1 == amount_ready_threads()) {
			scheduler_sleep();
		} else {
			scheduler_sleep();
		}
		next_thread();
		load_context(context);
		scheduler_wakeup();

		OS_PRINT ("\n\t\tFREDDULER:\n");
		LIST_DMP() ;
		if (NULL != context) {
			OS_PRINT ("\tcurrent context: \n");
		}

	} else {
		if (is_idling()) {
			OS_PRINT ("Lade idle thread!\n");
			load_idle_thread(context);
		}
		scheduler_sleep();
	}
}

void scheduler_wakeup() {
	OS_PRINT ("Timer enabled\n");
	arm_timer_enable();
}
void scheduler_sleep() {
	if (amount_sleeping_threads() <= 0) {
		OS_PRINT ("Timer disabled\n");
		arm_timer_disable();
	}
}

int scheduler_kill_thread(void * context) {
	if (NULL == context)
		return -1;
	thread_kill();
	schedule(context);
	arm_timer_reset();
	return 1;
}

int scheduler_create_thread(struct thread_context *ctx) {
	new_thread((void *) ctx->r[0], (void *) ctx->r[1], (size_t) ctx->r[2], ctx->r[3]);
	if (amount_ready_threads() < 2) {
		if (is_idling()) {
			schedule((void *) ctx);
		} else {
			scheduler_wakeup();
		}
	}
	return 1;
}

void scheduler_thread_sleep(struct thread_context *ctx) {
	safe_thread_context(ctx);
	thread_sleep(ctx->r[0]);
	schedule((void *) ctx);
	scheduler_wakeup();
}

void scheduler_thread_wait(struct thread_context *ctx) {
	safe_thread_context(ctx);
	thread_wait();
	schedule((void *) ctx);
}

void scheduler_init(struct thread_context *ctx) {
	if (0 != amount_ready_threads())
		schedule(ctx);
}
