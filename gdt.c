//intel 
#include "system.h" 
#define SEGMENT_COUNT 0x100
struct gdt_entry
{   unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access_right;
    unsigned char  granularity;
    unsigned char  base_high;
}__attribute__((packed));

struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
}__attribute__((packed));


struct gdt_entry gdt[SEGMENT_COUNT];
struct gdt_ptr gdtp;


void gdt_set_gate(int num, unsigned long base,unsigned long limit, unsigned char access,unsigned char gran)
{
     /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access_right = access;
}

void gdt_install()
{
    /* Setup the GDT pointer and limit */
    int i;
    gdtp.limit = (sizeof(struct gdt_entry) * SEGMENT_COUNT) - 1;
    gdtp.base = &gdt;
    for(i=0; i < SEGMENT_COUNT;i++)
    {
        gdt_set_gate(i, 0, 0, 0, 0);//我们初始化gdt
    }


    //0x92系统专用，可读写的段，不可执行
    //0x9a系统专用，可执行的段，可读不可写
    //0xf2应用程序用，可读写的段 
    //0xfa应用程序用，可读不可写
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_set_gate(2, 0x00280000, 0xFFFFFFFF, 0x9a, 0x40);

    /* Flush out the old GDT and install the new changes! */
    load_gdtr(&gdtp);
} 
