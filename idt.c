#include "system.h"
struct idt_entry
{   
    unsigned short base_low;
    unsigned short selector;
    unsigned char always0;
    unsigned char flags;
    unsigned short  base_hi;
}__attribute__((packed));


struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
}__attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;
//
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    idt[num].base_low = (base & 0xffff);
    idt[num].base_hi = ((base >> 16) & 0xff);
    idt[num].selector = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_install()
{
    int i;
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = &idt;
    for(i=0;i<256;i++)
    {
        idt_set_gate(i,0,0,0);
    }
	
	idt_set_gate(0x0c, (int) asm_inthandler0c, 2 * 8, 0x8E);
	idt_set_gate(0x0d, (int) asm_inthandler0d, 2 * 8, 0x8E); 
	idt_set_gate(0x20, (int) asm_inthandler20, 2 * 8, 0x8E);
    idt_set_gate(0x21, (int) asm_inthandler21, 2 * 8, 0x8E);
    idt_set_gate(0x27, (int) asm_inthandler27, 2 * 8, 0x8E);
    idt_set_gate(0x2c, (int) asm_inthandler2c, 2 * 8, 0x8E);
	idt_set_gate(0x40, (int) asm_hrb_api,      2 * 8, 0x8E + 0x60); 
    
    load_idtr(&idtp);
    
}