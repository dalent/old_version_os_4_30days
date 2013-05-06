#include "system.h"
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1


void init_pic(void)
{
	io_out8(PIC0_IMR,  0xff  );//禁用主pic中断
	io_out8(PIC1_IMR,  0xff  ); //禁用从pic中断

	io_out8(PIC0_ICW1, 0x11  ); //边沿触发模式
	io_out8(PIC0_ICW2, 0x20  ); //irq0-7由0x20-0x27接收
	io_out8(PIC0_ICW3, 1 << 2); //pic1由IRQ2接收
	io_out8(PIC0_ICW4, 0x01  ); //无缓冲区模式

	io_out8(PIC1_ICW1, 0x11  ); //边沿触发模式
	io_out8(PIC1_ICW2, 0x28  ); //IRQ。。
	io_out8(PIC1_ICW3, 2     ); //pic1由IRQ2接收
	io_out8(PIC1_ICW4, 0x01  ); //

	io_out8(PIC0_IMR,  0xfb  ); //pic1以外的全部禁止
	io_out8(PIC1_IMR,  0xff  ); //全部禁止

	return;
}


void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67); 
	return;
}