#include "system.h"


#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4
struct FIFO32* mousefifo;
int mousedata0;


void inthandler2c(int *esp)
{
	unsigned char data ;
	io_out8(PIC1_OCW2,0x64);
	io_out8(PIC0_OCW2,0x62);
	data = io_in8(PORT_KEYDAT);
	fifo32_put(mousefifo, data + mousedata0);
	return;
}

void enable_mouse(struct FIFO32* fifo, int data0, struct MOUSE_DEC *mdec)
{
	mousefifo = fifo;
	mousedata0 = data0;
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	mdec->mouse_phase = 0;
	return;
}

int mousedecode(struct MOUSE_DEC* mdec, unsigned char dat)
{
		if(mdec->mouse_phase == 0)
		{
			if(dat == 0xfa)
			{
				mdec->mouse_phase = 1;
			}
			return 0;
		}else if(mdec->mouse_phase == 1)
		{
			if((dat & 0xc8) == 0x08)
			{
				mdec->mouse_dbuf[0] = dat;
				mdec->mouse_phase = 2;
			}
			return 0;
		}else if(mdec->mouse_phase == 2)
		{
			mdec->mouse_dbuf[1] = dat;
			mdec->mouse_phase = 3;
			return 0;
		}else if(mdec->mouse_phase == 3)
		{
			mdec->mouse_dbuf[2] = dat;
			mdec->mouse_phase = 1;
			mdec->btn = mdec->mouse_dbuf[0] & 0x07;
			mdec->x = mdec->mouse_dbuf[1];
			mdec->y = mdec->mouse_dbuf[2];
			if((mdec->mouse_dbuf[0] & 0x10) !=0)
			{
				mdec->x |= 0xffffff00;
			}
			if((mdec->mouse_dbuf[0] & 0x20) != 0)
			{
				mdec->y |= 0xffffff00;
			}
			mdec->y = -mdec->y;
			return 1;
		}
		return -1;
}
