OUTPUT_FORMAT("binary")
phys = 0x00000000;
SECTIONS
{
  .text   0:  AT(phys)
  {
    code = .; _code = .; __code = .;
    *(.text) 
  }
  data = .; _data = .; __data = .;
  .data : AT(phys + data - code)
  {
	*(.rodata)
    *(.data)
	*(.bss)
	*(.*)
  }
  dataend = .; dataend = .; __datasize = .;

}