SECTIONS {
	.text :
	{
		_start = .;
		* (.start);
		* (.main);
		* (.text);
	}
	.data :
	{
		* (.data)
		. = ALIGN(0x1000);
		* (.sun4i-a10)
		. = ALIGN(0x1000);
		* (.sun5i-a10s)
		. = ALIGN(0x1000);
		* (.sun5i-a13)
		. = ALIGN(0x1000);
		* (.sun6i-a31)
		. = ALIGN(0x1000);
		* (.sun7i-a20)
		. = ALIGN(0x1000);
	}
	.rodata : { * (.rodata); }
	.bss :
	{
		* (.bss);
	}
	. = ALIGN(4);
	_edata  = .;

	_magic_start = _start;
	_magic_end = _edata;
}
