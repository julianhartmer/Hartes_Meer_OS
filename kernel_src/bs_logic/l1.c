#include "l1.h"
#include <stdint.h>
#include "bs_logic/k_io.h"
#include "driver/uart.h"

extern const char _kernel_ro_start, _kernel_ro_end, _kernel_rw_start, _kernel_rw_end, _user_ro_start,
       _user_ro_end, _user_rw_start, _user_rw_end;

#define PROCESS_DATA_SIZE (&_user_rw_end - &_user_rw_start)
#define PROCESS_DATA_ADR(X) (&_user_rw_start + X*(PROCESS_DATA_SIZE))
#define L1_SIZE 4096
#define RAM_MB_SIZE 128
#define PERI_START_MB	((GPIO_BASE_ADDR-GPIO_OFFSET)>>20)	// Log2(GPIO_BASE_ADDR-GPIO_OFFSET)
#define PERI_END_MB	((GPIO_END_ADDR-GPIO_OFFSET)>>20)	// Log2(GPIO_END_ADDR-GPIO_OFFSET)
#define ADDR_TO_MB(X)	((uint32_t) (X) >> 20)
#define MB_TO_ADDR(X)	((uint32_t) (X) << 20)
__attribute__((aligned(16384)))
	uint32_t l1_entry[L1_SIZE];

uint32_t current_t, current_p;

enum permission {
	fault	= -1,
	k0_u0	= 0,
	krw_u0	= 1,
	krw_ur	= 2,
	krw_urw = 3,
	kr_u0	= 5
};

int page_init(uint32_t * l1, uint32_t begin, uint32_t size, uint32_t start_addr, enum permission p);
void flush_thread_stack();

void * l1_init() {
	page_init(l1_entry, 0, L1_SIZE, 0, fault);		// set all to fault

	uint32_t start = ADDR_TO_MB (&_kernel_ro_start);
	uint32_t end = ADDR_TO_MB (&_kernel_ro_end);
	page_init(l1_entry, start, end, (uint32_t) &_kernel_ro_start, kr_u0);

	start = ADDR_TO_MB (&_kernel_rw_start);
	end = ADDR_TO_MB (&_kernel_rw_end);
	page_init(l1_entry, start, end, (uint32_t) &_kernel_rw_start, krw_u0);

	start = ADDR_TO_MB (&_user_ro_start);
	end = ADDR_TO_MB (&_user_ro_end);
	page_init(l1_entry, start, end, (uint32_t) &_user_ro_start, krw_ur);

//	start = ADDR_TO_MB (&_user_rw_start);
//	end = ADDR_TO_MB (&_user_rw_end);
//	page_init(l1_entry, start, end, (uint32_t) &_user_rw_start, krw_urw);

//	page_init(l1_entry, end+1, RAM_MB_SIZE, (uint32_t) &_user_ro_end + MB_TO_ADDR(1), krw_urw);
	page_init(l1_entry, PERI_START_MB, PERI_END_MB, GPIO_BASE_ADDR-GPIO_OFFSET, krw_u0);
	
	
	//Adding sp of priv modes
	page_init(l1_entry, ADDR_TO_MB(ABORT_SP - STACK_SIZE), ADDR_TO_MB(ABORT_SP), ABORT_REAL_SP, krw_u0);
	page_init(l1_entry, ADDR_TO_MB(FIQ_SP - STACK_SIZE), ADDR_TO_MB(FIQ_SP), FIQ_REAL_SP, krw_u0);
	page_init(l1_entry, ADDR_TO_MB(IRQ_SP - STACK_SIZE), ADDR_TO_MB(IRQ_SP), IRQ_REAL_SP, krw_u0);
	page_init(l1_entry, ADDR_TO_MB(SYSTEM_SP - STACK_SIZE), ADDR_TO_MB(SYSTEM_SP), SYSTEM_REAL_SP, krw_u0);
	page_init(l1_entry, ADDR_TO_MB(UDI_SP - STACK_SIZE), ADDR_TO_MB(UDI_SP), UDI_REAL_SP, krw_u0);
	page_init(l1_entry, ADDR_TO_MB(SUPER_SP - STACK_SIZE), ADDR_TO_MB(SUPER_SP), SUPER_REAL_SP, krw_u0);

	//Adding thread sp
	for (int i = 0; i<THREAD_AMOUNT; i++) 
		page_init(l1_entry, ADDR_TO_MB(THREAD_SP (i) - STACK_SIZE), ADDR_TO_MB (THREAD_SP (i)), THREAD_REAL_SP (i), krw_u0);
//	for (int i = 0; i <= PERI_END_MB; i++)
//		kprintf("l1[%i]\t=%x (%x)\n",i, l1_entry[i], i<<20);

	return l1_entry;
}

int page_init(uint32_t *l1, uint32_t begin, uint32_t end, uint32_t start_addr, enum permission p) {
	uint32_t i;
	for (i = 0; i < end-begin; i++) {
		if (p < 0) {
			l1[begin+i] = 0;
		} else {
			l1[begin+i] = start_addr  | (i<<20) | (p<<10) | 2;

//			if (p) kprintf("adding entry l1[%i]\t= %x, VA: %x, RA: %x\n", begin+i, l1[begin+i], (begin+i)<<20, start_addr+(i<<20));
		}
	}
	return i;
}

void change_process_addr(uint32_t t_id, uint32_t p_id) {
	uint32_t start = ADDR_TO_MB (&_user_rw_start);
	uint32_t end = ADDR_TO_MB (&_user_rw_end);
	flush_thread_stack();
	page_init(l1_entry, start, end, (uint32_t) PROCESS_DATA_ADR (p_id), krw_urw);
	__asm("mcr p15, 0, r0, c8, c7, 0");	// clear tlb
	//kprintf("\tuser stack: an adresse %x\n", THREAD_REAL_SP (t_id));
	//kprintf("\tl1 beim user stack %x\n", l1_entry[ADDR_TO_MB(THREAD_SP (t_id) - STACK_SIZE)]);
	page_init(l1_entry, ADDR_TO_MB(THREAD_SP (t_id) - STACK_SIZE), ADDR_TO_MB (THREAD_SP (t_id)), THREAD_REAL_SP (t_id), krw_urw);

	//kprintf("\tloading process %i (start = %i, end  = %i, addr %x) thread %i\n", p_id, start, end, PROCESS_DATA_ADR (p_id), t_id);
	current_t = t_id;
	current_p = p_id;
}

void flush_thread_stack() {
	page_init(l1_entry, ADDR_TO_MB(THREAD_SP (current_t) - STACK_SIZE), ADDR_TO_MB (THREAD_SP (current_t)), THREAD_REAL_SP (current_t), krw_u0);
	current_t = -1;
}
