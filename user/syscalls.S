	.globl sys_create_thread
	.globl sys_kill_thread
	.globl sys_sleep_thread
	.globl sys_send_char
	.globl sys_read_char

sys_create_thread:
	push	{lr}
	swi	#0
	pop	{pc}

sys_kill_thread:
	push	{lr}
	swi	#1
	pop	{pc}

sys_sleep_thread:
	push	{lr}
	swi	#2
	pop	{pc}

sys_send_char:
	push	{lr}
	swi	#3
	pop	{pc}

sys_read_char:
	push	{lr}
	swi	#4
	pop	{pc}
