ENTRY(_start)
SECTIONS
{
	. = 0x100000;
	PROVIDE (_kernel_ro_start = .);
	kernel_ro : {
		kernel_src/?*(.init)
		kernel_src/?*(.text)
		kernel_src/?*(.rodata*)
	}
	. = ALIGN(0x100000);
	PROVIDE (_kernel_ro_end = .);

	PROVIDE (_kernel_rw_start = .);
	kernel_rw : {
		kernel_src/?*(.data)
		kernel_src/?*(.bss) 
		kernel_src/?*(COMMON)
	}	
	. = ALIGN(0x100000);
	PROVIDE (_kernel_rw_end = .);

	PROVIDE (_user_ro_start = .);
	user_ro : { 
		user/?*(.text)
		user/?*(.rodata*) 
	}
	. = ALIGN(0x100000);
	PROVIDE (_user_ro_end = .);

	PROVIDE (_user_rw_start = .);
	user_rw ALIGN(0x100000) : {
		user/?*(.data)
		user/?*(.bss) 
		user/?*(COMMON) 
	}
	. = ALIGN(0x100000);
	PROVIDE (_user_rw_end = .);

}

