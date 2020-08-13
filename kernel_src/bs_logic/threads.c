#include <stdint.h>
#include "threads.h"

#include "bs_logic/l1.h"
#include "bs_logic/process.h"
#include "bs_logic/structures/list.h"
#include "bs_logic/k_io.h"
#include "bs_logic/syscalls.h"

#include "bs_logic/context.h"

#include "driver/addresses.h"
#define STACK_ALIGN 4
#define TCB_2_LIST_ELEM(X) ((volatile struct list_elem *) X)
#define LIST_ELEM_2_TCB(X) ((volatile struct tcb *) X)

void idle_thread(void * data);

struct tcb {
	struct list_elem rq;
	char* basis_sp;
	int p_id;
	int t_id;
	volatile struct thread_context context;
};

volatile struct tcb tcbs[THREAD_AMOUNT];
// Queue (FIFO-List) of ready threads
volatile struct list_elem *ready_q;
// Queue (FIFO-List) of free tcb entries
volatile struct list_elem *free_q;
// pointer to currently running thread
volatile struct list_elem *running_fredd;
// Remaining Queue of sleeping tcb entries
volatile struct list_elem *sleep_q;
// Fifo Queue of waiting threads (waiting for char)
volatile struct list_elem *wait_q;


int new_thread(void *func, void *data, size_t len, int new_process) {
	int p_id;
	if (!new_process) {
		p_id = LIST_ELEM_2_TCB(running_fredd)->p_id;
		add_thread_to_process(p_id);
	} else {
		p_id = get_free_process();
	}
	if (p_id < 0) {
		kprintf("Couldnt spawn process, all in use!\n");
		return -1;
	}
	volatile struct tcb *tcb_sel = LIST_ELEM_2_TCB (pop_first(&free_q));
	if (NULL == tcb_sel) {
		return 0;	// Cant spawn thread, all threads in use
	}
	tcb_sel->context.sp = tcb_sel->basis_sp - len;
	tcb_sel->context.sp -= ((uintptr_t)tcb_sel->context.sp % STACK_ALIGN);
	for (size_t i = 0; i < len; i++)
		tcb_sel->context.sp[i] = ((char *) data)[i];
	tcb_sel->context.pc = func;
	tcb_sel->context.r[0] = (uint32_t) tcb_sel->context.sp;
	tcb_sel->context.lr = (uint32_t) sys_kill_thread;
	tcb_sel->context.cpsr = 1<<4;
	push_last(&ready_q, TCB_2_LIST_ELEM (tcb_sel));
	tcb_sel->p_id = p_id;

	return 1;
}

// amount > 0: sleep for amount time slices
// amount = 0: switch context
void thread_sleep(int amount) {
	if (NULL == running_fredd) {
		OS_ERROR ("Trying to put thread asleep, but not thread present!");
	}
	OS_MSG ("sleeping for %i ticks!\n", amount);
	volatile struct list_elem *tmp = running_fredd;
	running_fredd = NULL;
	tmp->remaining = amount;
	rem_push(&sleep_q, tmp);
}

void thread_wait() {
	if (NULL == running_fredd) {
		OS_ERROR("Trying to put thread await, but no thread present!");
	}
	volatile struct list_elem *tmp = running_fredd;
	running_fredd = NULL;
	push_last(&wait_q, tmp);
}

void init_threads(void *func, void *data, size_t len){
	ready_q = NULL;
	running_fredd = NULL;
	sleep_q = NULL;
	wait_q = NULL;
	push_last(&free_q, &tcbs[0].rq);
	tcbs[0].t_id = 0;
	LIST_ELEM_2_TCB (free_q)->basis_sp = (char *) THREAD_SP (0);
	for (int i = 1; i < THREAD_AMOUNT; i++) {
		push_last(&free_q, &tcbs[i].rq);
		tcbs[i].basis_sp =
			(char *) (THREAD_SP (i));
		tcbs[i].t_id = i;
	}
	if (NULL != func) {
		new_thread(func, data, len, 1);
	}
}

void thread_kill() {
	if (NULL == running_fredd) {
		kprintf("Critical Error: Tried to kill thread which isnt present\n");
		while(1);
		__asm__("udf");
		return;
	}
	push_first(&free_q, running_fredd);
	free_process((LIST_ELEM_2_TCB (running_fredd)->p_id));
	running_fredd = NULL;
}


int amount_sleeping_threads() {
	return list_size(sleep_q);
}

int amount_waiting_threads() {
	return list_size(wait_q);
}

int amount_ready_threads() {
	return list_size(ready_q);
}

int amount_free_threads() {
	return list_size(free_q);
}


void safe_thread_context(void * ctx_p) {
	if (NULL == ctx_p || NULL == running_fredd)
		return;
	volatile struct thread_context * t_ctx =
		&(LIST_ELEM_2_TCB (running_fredd)->context);
	volatile struct thread_context * ctx =
		(volatile struct thread_context *) ctx_p;
	for (int i = 0; i < GP_REGISTER_AMOUNT; i++)
		t_ctx->r[i] = ctx->r[i];
	t_ctx->pc = ctx->pc;
	t_ctx->sp = ctx->sp;
	t_ctx->lr = ctx->lr;
       	t_ctx->cpsr = ctx->cpsr;
}


void next_thread() {
	volatile struct list_elem * temp = pop_first(&ready_q);
	// found a new thread
	if (NULL != temp) {
		// do I need to store currently running thread?
		if (running_fredd != NULL)
			push_last(&ready_q, running_fredd);
		// set new thread as running thread
		running_fredd = temp;
	}
}

void load_context(void * ctx_pointer) {
	struct thread_context * ctx = (struct thread_context *) ctx_pointer;
	if (NULL == running_fredd) {
		OS_ERROR("No thread ready!");
	}
	volatile struct thread_context * new_ctx = &(LIST_ELEM_2_TCB (running_fredd)->context);
	for (int i = 0; i < GP_REGISTER_AMOUNT; i++)
		ctx->r[i] = new_ctx->r[i];
	ctx->sp = new_ctx->sp;
	ctx->lr = new_ctx->lr;
	ctx->pc = new_ctx->pc;
	ctx->cpsr = new_ctx->cpsr;
	// TODO change l1 accordingly
	change_process_addr(LIST_ELEM_2_TCB (running_fredd)->t_id, LIST_ELEM_2_TCB (running_fredd)->p_id);
}

void load_idle_thread(volatile void *ctx_pointer) {
	// TODO change l1 accordingly
	running_fredd = NULL;
	volatile struct thread_context * ctx = (volatile struct thread_context *) ctx_pointer;
	for (int i = 0; i < GP_REGISTER_AMOUNT; i++)
		ctx->r[i] = 0;
	// sp and  lr will never be user
	// if they are used, they will cause a data abort
	ctx->pc = idle_thread;
	ctx->cpsr = 31;
//	ctx->sp = (char *) ~((uintptr_t) ctx->sp & 0);
	ctx->sp = (char *) ((uintptr_t) 0);
	ctx->lr = ~((uint32_t) ctx->sp & 0);
}

void print_context(void *ctx_pointer) {
	struct thread_context * ctx = (struct thread_context *) ctx_pointer;
	if (NULL == ctx) return;
	kprintf("\nCONTEXT DUMB\n");
	for (int i = 0; i < GP_REGISTER_AMOUNT; i++)
		kprintf("\tr[%u]: %x\n", i, ctx->r[i]);
	kprintf("\tsp: %x\n", ctx->sp);
	kprintf("\tpc: %x\n", ctx->pc);
	kprintf("\tlr: %x\n", ctx->lr);
	kprintf("\tcpsr: %x\n", ctx->cpsr);
	kprintf("\n");
}

int is_idling() {
	if (NULL == running_fredd)
		return 1;
	return 0;
}

void thread_finish() {
	__asm__ ("svc 1");
}

void poke_sleeping_threads() {
	rem_tick(sleep_q);
	OS_MSG ("\n still need to wait %i\n", sleep_q->remaining);
	volatile struct list_elem *woke;
	while(NULL != (woke = rem_pop_fin(&sleep_q))) {
		push_first(&ready_q, woke);
	}
}

int feed_char_to_thread(char c) {
	volatile struct list_elem *woke = pop_first(&wait_q);
	if (NULL == woke) return 0;
	LIST_ELEM_2_TCB(woke)->context.r[0] = c;
	push_first(&ready_q, woke);
	OS_MSG ("Thread is WOKE!\n");
	return 1;
}
