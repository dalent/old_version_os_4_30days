OUTPUT_FORMAT("binary")
phys = 0x00000000;
SECTIONS
{
  .text   0:  AT(phys)
  {
    code = .;
    *(.text) 
  }
  data = .;
  .data 0: AT(phys + data - code)
  {
	*(.rodata)
    *(.data)
	*(.bss)
	*(.*)
  }

}