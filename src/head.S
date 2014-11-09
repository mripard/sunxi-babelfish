	.arch	armv7-a

	.align
	.section ".start", "ax"
	.arm			@ Always enter in ARM state
_start:
	.rept	8
	mov	r0, r0
	.endr
	b	1f

	.word	0x016f2818	@ Magic numbers to help the loader
	.word	_magic_start	@ absolute load/run zImage address
	.word	_magic_end	@ zImage end address
	.word	0x04030201	@ endianness flag

1:
	b	main
