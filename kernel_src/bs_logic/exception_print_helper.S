.global mode_regs

mode_regs:
	mrs	r2, CPSR
	cps	#31		@ USER/SYSTEM
	stmia	r0!, {sp, lr}
	CPS	#19     	@ SUPERVISOR
	stmia	r0!, {sp, lr}
	CPS	#23     	@ ABORT
	stmia	r0!, {sp, lr}
	CPS	#17     	@ FIQ
	stmia	r0!, {sp, lr}
	CPS	#18     	@ IRQ
	stmia	r0!, {sp, lr}
	CPS	#27     	@ UDI
	stmia	r0!, {sp, lr}

	msr	CPSR, r2
	bx	lr
