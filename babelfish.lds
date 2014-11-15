SECTIONS {
	 /DISCARD/ : {
		* (.data);
		* (.data.*);
	}

	.text :
	{
		_start = .;
		* (.start);
		* (.main);
		* (.text);
	}

	.dtbs :
	{
		. = ALIGN(4);
		* (.dtbs.sun4i-a10)
		. = ALIGN(4);
		* (.dtbs.sun5i-a10s)
		. = ALIGN(4);
		* (.dtbs.sun5i-a13)
		. = ALIGN(4);
		* (.dtbs.sun6i-a31)
		. = ALIGN(4);
		* (.dtbs.sun7i-a20)
	}

	.zimage :
	{
		. = ALIGN(4);
		* (.zimage);
	}

	.rodata :
	{
		* (.rodata);
	}

	. = ALIGN(4);
	_edata  = .;

	.bss :
	{
		* (.bss);
	}

	.heap :
	{
		* (.heap);
	}

	_magic_start = _start;
	_magic_end = _edata;
}
