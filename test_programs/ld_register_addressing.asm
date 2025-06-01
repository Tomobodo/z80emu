	LD A, 0x24; 7 t_cycles
	LD B, A; 11
	LD C, A; 15
	LD D, A; 19
	LD E, A; 23
	LD H, A; 27
	LD L, A; 31

	LD B, 0x47; 38
	LD A, B; 42
	LD C, B; 46
	LD D, B; 50
	LD E, B; 54
	LD H, B; 58
	LD L, B; 62

	HALT
