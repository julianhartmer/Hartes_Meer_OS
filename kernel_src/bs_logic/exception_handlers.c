#include <stdint.h>
#include "bs_logic/k_io.h"

#include "driver/uart.h"
#include "driver/peripherals_ctrl.h"
#include "driver/arm_timer.h"

#include "bs_logic/exception_print.h"
#include "bs_logic/scheduler.h"
#include "bs_logic/threads.h"

#include "bs_logic/syscalls.h"
#include "bs_logic/context.h"

#define UDI_PC_OFFSET 1
#define SWI_PC_OFFSET 1
#define DA_PC_OFFSET 2
#define CPU_MODE_BITS (1 + (1<<1) + (1<<2) + (1<<3) + (1<<4))
#define CPU_USER_MODE (1<<4)
#define SWI_NUMBER_MASK 0xFF

void user_thread(void *x);

struct register_params_simple {
	uint32_t pc;
	uint32_t cpsr;
	uint32_t spsr;
	uint32_t *lr;
};


//return 1 on system halt, returns 0 on killing user thread
void undefined_instruction(struct thread_context * ctx) {
	if ((ctx->cpsr & CPU_MODE_BITS) == CPU_USER_MODE) {
		kprintf("\n\tOS: Undefined Instruction! Killing User-Thread!\n");
		thread_kill();
		schedule(ctx);
		return;
	}
	kprintf("%s\n",SEPERATOR);
	kprintf("ERROR: Undefined Instruction an Adresse %x\
			\nFolgende Instruktion nicht ünterstützt %x\n\n\
			SYSTEM WIRD ANGEHALTEN!\n", ctx->pc, *((uint32_t *)ctx->pc));
	print_crash(ctx);
}

void software_interrupt(struct thread_context * ctx) {
	if ((ctx->cpsr & CPU_MODE_BITS) != CPU_USER_MODE) {
		kprintf("%s\n",SEPERATOR);
		kprintf("ERROR: OS Trying a Syscall!\n\
			\nSyscall at Address: %x\n\n\
			SYSTEM WILL BE HALTED!\n", ctx->pc, *((uint32_t *)ctx->pc));
		print_crash(ctx);
		return;
	}
	int number = ((uint32_t *)(ctx->pc))[-1] & SWI_NUMBER_MASK;
	char x;
	switch (number) {
		case 0:	
			ctx->r[0] = scheduler_create_thread(ctx);
			return;
		case 1: ctx->r[0] = scheduler_kill_thread(ctx);
			return;
		case 2: OS_MSG ("Trying to sleep!\n");
			scheduler_thread_sleep(ctx);
			return;
			
		case 3: send_char(ctx->r[0]);
			return;
		case 4:
			OS_MSG ("read_char...\n");
			x = pop_char();
			if (0 == x) {
				OS_MSG ("no char available, setting to sleep for char!\n");
				scheduler_thread_wait(ctx);
				return;
			} 
			OS_MSG ("got char %i\n", x);
			ctx->r[0] = x;
			return;
		case 5:
			OS_MSG ("Open channel...\n");
			break;
		case 6:
			OS_MSG ("Send channel...\n");
			break;
		case 7:
			OS_MSG ("Recieve channel...\n");
			break;
		default: kprintf("SYSCALL %i NOT IMPLEMENTED!", number);
	}
	kprintf("\n\tOS: Undefined syscall! Killing User-Thread!\n");
	thread_kill();
	schedule(ctx);
	return;
}

//return 1 on system halt, returns 0 on killing user thread
void data_abort(void *ctx_p, uint32_t dfsr, uint32_t dfar) {
	struct thread_context *ctx = ctx_p;
	if ((ctx->cpsr & CPU_MODE_BITS) == CPU_USER_MODE) {
		kprintf("\n\tOS: User-Thread Data error! Killing User-Thread!\n");
		if (ctx->pc) kprintf("\nData abort at Address: %x\n\n", ctx->pc, *((uint32_t *)ctx->pc));
		else kprintf("Jump to Nullpointer!\n");
		
			
		print_exc_context(ctx);
		print_abort_type(dfsr, dfar);
		thread_kill();
		kprintf("\nOS loading other thread...\n\n\n");
		schedule(ctx);
		return;
	}
	kprintf(SEPERATOR);
	kprintf("ERROR: OS caused data abort!\n\
		\nData abort at Address: %x\n\n\
		SYSTEM WILL BE HALTED!\n", ctx->pc, *((uint32_t *)ctx->pc));
	print_abort_type(dfsr, dfar);
	print_crash(ctx);

	return;
}

void irq(void *context) {
//	kprintf ("got irq\n");
	switch(per_get_irq_type()) {
	case arm_timer:
		//poke_sleeping_threads();
		schedule(context);
		arm_timer_ack();
		break;
	case uart:
		uart_handle_recieve();
		if (feed_char_to_thread(peek_char())) {
			char c = pop_char();
			if ('\0' != c)
				OS_MSG ("fed char, %c\n", c);
		}
		schedule(context);
		break;
	// Do nothing on spurious wakeups
	case undefined:
		OS_MSG ("Spurious Wakeup!");
		break;
	}
}
