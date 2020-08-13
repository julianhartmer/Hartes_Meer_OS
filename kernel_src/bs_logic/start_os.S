#include "driver/addresses.h"
#include "driver/mmu.h"
.section .init

.global _start
_start:
// ram ends in 0x8000000 (128 mib)
// each mode gets 1mib stack

	@ set IVT
	ldr	r0, =IVT_START
	@ activate IVT
	mcr	p15, #0, r0, c12, c0, #0

	ldr	sp, =SYSTEM_REAL_SP

	bl	enable_uart_interrupts
	bl	uart_init_recieve_interrupts

	mcr	p15, 0, r0, c1, c0, 0	@ read sctlr (p. 1937)
	and	r0, r0, #SCTLR_DIS_CACHE
	mrc	p15, 0, r0, c1, c0, 0	@ write sctlr (p. 1937)
	bl	l1_init
	MCR	p15, 0, r0, c2, c0, 0	@ set TTBR0 to L1 Address (p. 1732)
//	MCR	p15, 0, r0, c2, c0, 1	@ set TTBR1 to L1 Address (p. 1734)

	ldr	r0, =#0
	MCR	p15, 0, r0, c2, c0, 2	@ set TTBCR to short descriptor format (p. 1728)
	ldr	r0, =DACR_EN
	MCR	p15, 0, r0, c3, c0, 0
	MRC	p15, 0, r1, c1, c0, 0	@ get mmu control register (p. 1707)
	ORR	r1, #0x1		@ set m bit to 0
	mcr	p15, 0, r1, c1, c0, 0	@ activate mmu


	@ initialize virtual stackpointers for different modes
	CPS	#23     	@ ABORT
	ldr	sp, =ABORT_SP
	CPS	#17     	@ FIQ
	ldr	sp, =FIQ_SP
	CPS	#18     	@ IRQ
	ldr	sp, =IRQ_SP
	CPS	#27     	@ UDI
	ldr	sp, =UDI_SP
	CPS	#19     	@ SUPERVISOR
	ldr	sp, =SUPER_SP
	CPS	#31		@ SYSTEM
	ldr	sp, =SYSTEM_SP

	// set timer to 0.10 secs
	// this site suggests time slice size in the range [10,100] ms https://www2.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/6_CPU_Scheduling.html
	// Because we have to print "!", our context switch is extremly slowed down. Thats why i choose 100ms.
	// Becuase the context switch time is way longer than usual, the time slice time should be higher as well.
	// Note: 100ms is the default linux rr-time slice (compare include/linux/sched/rt.h).
	ldr	r0, =#1000
	mov	r1, #0
	mov	r2, #0
	bl	arm_timer_init
	bl	enable_arm_timer_interrupts

	ldr	r0, =main	@ set main as first user thread
	ldr	r2, =0		@ no data for main function

	CPS	#19     	@ SUPERVISOR
	sub	sp, sp, #68
	bl	init_threads
	mov	r0, sp
	bl	schedule

	ldm	sp, {sp,lr}^
	nop

	add	sp, sp, #8
	pop	{r2, lr}
	msr	spsr, r2
	pop	{r0-r12}
	movs	pc, lr
