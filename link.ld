OUTPUT_FORMAT("binary")
phys = 0x00000000;
phys1 = 0x00280000;
SECTIONS
{
  .text  phys :  AT(phys)
  {
    code = .;
    *(.text) 
  }
  data = .;
  .data phys1 + data   : AT(phys + data - code)
  {
	*(.rodata)
    *(.data)
	*(.bss)
	*(.*)
  }

}