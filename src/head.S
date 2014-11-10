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
	adr	r3, _start	@ Retrieve the address we've been
				@ started from

	ldr	sp, =stack_end	@ Initialize the stack pointer at
				@ the (relative) end of the section

	sub	sp, sp, #4	@ The end pointer is the same than the
				@ next section start pointer. In order
				@ to avoid conflicting with the heap,
				@ decrement the stack pointer

	add	sp, sp, r3	@ Increment the address of the stack
				@ by the absolute address we were
				@ started at

	ldr	r4, =heap	@ Retrieve our heap (relative) base address
	add	r4, r4, r3	@ Increment it by the absolute offset
	ldr	r5, =heap_base	@ Retrieve the heap base variable
	str	r4, [r5]	@ Store our heap pointer

	ldr	r4, =exec_base	@ Retrieve the execution base pointer
	str	r3, [r4]	@ And store it for our C friends

	b	main


	.global	exec_base
exec_base:
	.word	0x0

	.global	heap_base
heap_base:
	.word	0x0

	.align
	.section ".stack", "aw", %nobits
stack:
	.space	4096
stack_end:

	.align
	.section ".heap", "aw", %nobits
heap:
	.space	4096
heap_end:
