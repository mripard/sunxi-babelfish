SECTIONS {
	.text : { * (.text_main); * (.text); }
	.data :
	{
		* (.data)
		. = ALIGN(0x1000);
	}
	.rodata : { * (.rodata); }
	.bss :
	{
		* (.bss);
	}
}
