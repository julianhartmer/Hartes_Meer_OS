#include "exception_print.h"
#include "bs_logic/k_io.h"

#define CPSR_N_BIT (1<<31)
#define CPSR_Z_BIT (1<<30)
#define CPSR_C_BIT (1<<29)
#define CPSR_V_BIT (1<<28)
#define CPSR_I_BIT (1<<7)
#define CPSR_F_BIT (1<<6)
#define CPSR_T_BIT (1<<5)

#define DFSR_WNR (1<<11)
#define DFSR_TO_ERROR_CODE(X) ( (X&(1<<10)>>6) + (X&15))
#define DFSR_ERROR_ALIGNMENT_FAULT 1
#define DFSR_ERROR_CACHE 4
#define DFSR_ERROR_SYNC_EXT_ABORT_TWALK_1 12
#define DFSR_ERROR_SYNC_EXT_ABORT_TWAKL_2 14
#define DFSR_ERROR_SYNC_PARITY_1 28
#define DFSR_ERROR_SYNC_PARITY_2 30
#define DFSR_ERROR_TRANSLATION_1 5
#define DFSR_ERROR_TRANSLATION_2 7
#define DFSR_ERROR_ACCES_FLAG_1 3
#define DFSR_ERROR_ACCES_FLAG_2 6
#define DFSR_ERROR_DOMAIN_1 9
#define DFSR_ERROR_DOMAIN_2 11
#define DFSR_ERROR_PERMISSION_1 13
#define DFSR_ERROR_PERMISSION_2 15
#define DFSR_ERROR_DEBUG 2
#define DFSR_ERROR_SYNC_EX_ABORT 8
#define DFSR_ERROR_TLB 16
#define DFSR_ERROR_IMPL_DEF_LCKDWN 20
#define DFSR_ERROR_IMPL_DEF_COPR_ABORT 26
#define DFSR_ERROR_SYNC_PAR_MEM_ACCESS 25
#define DFSR_ERROR_ASYNC_EX_ABORT 22
#define DFSR_ERROR_ASYNC_PARITY 24
#define CPSR_USER_MODE (1<<4)
#define CPSR_FIQ ((1<<4) + 1)
#define CPSR_IRQ ((1<<4) + (1<<1))
#define CPSR_SVC ((1<<4) + (1<<1) + (1<<0))
#define CPSR_ABORT ((1<<4) + (1<<2) + (1<<1) + 1)
#define CPSR_UNDEFINED ((1<<4) + (1<<3) + (1<<1) + 1)
#define CPSR_SYSTEM ((1<<4) + (1<<3) + (1<<2) +(1<<1) + 1)
#define CPSR_MODE_BITS ((1<<4) + (1<<3) + (1<<2) +(1<<1) + 1)
struct register_params {
	uint32_t pc;
	uint32_t cpsr;
	uint32_t spsr;
	uint32_t *lr;
	uint32_t r[13];
};

void print_abort_type(uint32_t dfsr, uint32_t dfar) {
	kprintf("Zugriff %s auf Adresse %x\n", dfsr&DFSR_WNR?"schreibend":"lesend", dfar);
	kprintf("Fehler: ");
	uint32_t error = DFSR_TO_ERROR_CODE(dfsr);
	switch (error) {
	case DFSR_ERROR_ALIGNMENT_FAULT:
		kprintf("Alignment Fault\n");
		break;
	case DFSR_ERROR_CACHE:
		kprintf("Fault on instruction cache maintenance\n");
		break;
	case DFSR_ERROR_SYNC_EXT_ABORT_TWALK_1:
		kprintf("First Level Synchronous external abort on translation table walk\n");
		break;
	case DFSR_ERROR_SYNC_EXT_ABORT_TWAKL_2:
		kprintf("Second Level Synchronous external abort on translation table walk\n");
		break;
	case DFSR_ERROR_SYNC_PARITY_1:
		kprintf("First Level Synchronous parity error on translation table walk\n");
		break;
	case DFSR_ERROR_SYNC_PARITY_2:
		kprintf("Second Level Synchronous parity error on translation table walk\n");
		break;
	case DFSR_ERROR_TRANSLATION_1:
		kprintf("First Level Translation fault (MMU)\n");
		break;
	case DFSR_ERROR_TRANSLATION_2:
		kprintf("Second Level Translation fault (MMU)\n");
		break;
	case DFSR_ERROR_ACCES_FLAG_1:
		kprintf("First Level Access flag fault (MMU)\n");
		break;
	case DFSR_ERROR_ACCES_FLAG_2:
		kprintf("Second Level Access flag fault (MMU)\n");
		break;
	case DFSR_ERROR_DOMAIN_1:
		kprintf("First Level Domain fault (MMU)\n");
		break;
	case DFSR_ERROR_DOMAIN_2:
		kprintf("Second Level Domain fault (MMU)\n");
		break;
	case DFSR_ERROR_PERMISSION_1:
		kprintf("First Level Permission fault (MMU)\n");
		break;
	case DFSR_ERROR_PERMISSION_2:
		kprintf("Second Level Permission fault (MMU)\n");
		break;
	case DFSR_ERROR_DEBUG:
		kprintf("Debug event\n");
		break;
	case DFSR_ERROR_SYNC_EX_ABORT:
		kprintf("Synchronous external abort\n");
		break;
	case DFSR_ERROR_TLB:
		kprintf("TLB conflict abort\n");
		break;
	case DFSR_ERROR_IMPL_DEF_LCKDWN:
		kprintf("IMPLEMENTATION DEFINED - Lockdown\n");
		break;
	case DFSR_ERROR_IMPL_DEF_COPR_ABORT:
		kprintf("IMPLEMENTATION DEFINED - Coprocessor abort\n");
		break;
	case DFSR_ERROR_SYNC_PAR_MEM_ACCESS:
		kprintf("Synchronous parity error on memory access");
		break;
	case DFSR_ERROR_ASYNC_EX_ABORT:
		kprintf("Asynchronous external abort\n");
		break;
	case DFSR_ERROR_ASYNC_PARITY:
		kprintf("Asynchronous parity error on memory access\n");
		break;
	default:
		kprintf("Unknown Error, too bad LOL (Code: %u)\n", error);
	}

}

void print_exc_context(void *ctx_p) {
	struct thread_context *ctx = (struct thread_context *) ctx_p;
	kprintf(">>> Registerschnappschuss <<<\n");

	// Print R[0]-R[4] and R[8]-R[12], 2 in one line
	for (int i = 0, j = 8; i < 5; i++, j++) 
		kprintf("R%i:\t%x\tR%i:\t%x\n", i, ctx->r[i], j, ctx->r[j]);
	// Print remaining registers manualy
	kprintf("R5:\t%x\tSP:\t%x\n", ctx->r[5], ctx);
	kprintf("R6:\t%x\tLR:\t%x\n", ctx->r[6], ctx->lr);
	kprintf("R7:\t%x\tPC:\t%x\n", ctx->r[7], ctx->pc);
	kprintf("\n");

	kprintf(">>> Aktuelle Statusregister (SPSR des aktuellen Modus) <<<\n");
	kprintf("CPSR: %c", ctx->cpsr & CPSR_N_BIT ? 'N' : '_');
	kprintf("%c",  ctx->cpsr & CPSR_Z_BIT ? 'Z' : '_');
	kprintf("%c",  ctx->cpsr & CPSR_C_BIT ? 'C' : '_');
	kprintf("%c ", ctx->cpsr & CPSR_V_BIT ? 'V' : '_');
	kprintf("%c",  ctx->cpsr & CPSR_I_BIT ? 'I' : '_');
	kprintf("%c",  ctx->cpsr & CPSR_F_BIT ? 'F' : '_');
	kprintf("%c (%x): ", ctx->cpsr & CPSR_T_BIT ? 'T' : '_', ctx->cpsr);
	switch (ctx->cpsr & CPSR_MODE_BITS) {
	case CPSR_USER_MODE:
		kprintf("User Mode");
		break;
	case CPSR_FIQ:
		kprintf("FIQ");
		break;
	case CPSR_IRQ:
		kprintf("IRQ");
		break;
	case CPSR_SVC:
		kprintf("SVC");
		break;
	case CPSR_ABORT:
		kprintf("Abort");
		break;
	case CPSR_UNDEFINED:
		kprintf("Undefined");
		break;
	case CPSR_SYSTEM:
		kprintf("System");
		break;
	default: kprintf("!!!UNKNOWN!!!");
	}
//	kprintf("\nSPSR: %c", regs->spsr & CPSR_N_BIT ? 'N' : '_');
//	kprintf("%c",  regs->spsr & CPSR_Z_BIT ? 'Z' : '_');
//	kprintf("%c",  regs->spsr & CPSR_C_BIT ? 'C' : '_');
//	kprintf("%c ", regs->spsr & CPSR_V_BIT ? 'V' : '_');
//	kprintf("%c",  regs->spsr & CPSR_I_BIT ? 'I' : '_');
//	kprintf("%c",  regs->spsr & CPSR_F_BIT ? 'F' : '_');
//	kprintf("%c (%x): ", regs->spsr & CPSR_T_BIT ? 'T' : '_', regs->spsr);
//	switch (regs->spsr & CPSR_MODE_BITS) {
//	case CPSR_USER_MODE:
//		kprintf("User Mode");
//		break;
//	case CPSR_FIQ:
//		kprintf("FIQ");
//		break;
//	case CPSR_IRQ:
//		kprintf("IRQ");
//		break;
//	case CPSR_SVC:
//		kprintf("SVC");
//		break;
//	case CPSR_ABORT:
//		kprintf("Abort");
//		break;
//	case CPSR_UNDEFINED:
//		kprintf("Undefined");
//		break;
//	case CPSR_SYSTEM:
//		kprintf("System");
//		break;
//	default: kprintf("!!!UNKNOWN!!!");
//	}
	kprintf("\n");
}

void print_mode_registers(void *regs_p) {
	struct mode_register *regs = (struct mode_register *) regs_p;
	kprintf(">>> Aktuelle modusspezifische Register (außer SPSR, R8-R12) <<<\n");
	kprintf("             LR         SP\n");
	kprintf("User/System: %x %x\n", regs->user_lr, regs->user_sp);
	kprintf("Supervisor:  %x %x\n", regs->supervisor_lr, regs->supervisor_sp);
	kprintf("Abort:       %x %x\n", regs->abort_lr, regs->abort_sp);
	kprintf("FIQ:         %x %x\n", regs->fiq_lr, regs->fiq_sp);
	kprintf("IRQ:         %x %x\n", regs->irq_lr, regs->irq_sp);
	kprintf("Undefined:   %x %x\n", regs->udi_lr, regs->udi_sp);
	kprintf(SEPERATOR);
}

void print_crash(void * ctx_p) {
	struct thread_context *ctx = ctx_p;
	print_exc_context(ctx);
	struct mode_register regs;
	mode_regs(&regs);
	print_mode_registers(&regs);
	kprintf("Terminating OS! Farewell, and thanks for all the bugs!");
	while (1);
}

