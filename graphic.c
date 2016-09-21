#include "system.h"   


void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑色 */
		0xff, 0x00, 0x00,	/*  1:亮红*/
		0x00, 0xff, 0x00,	/*  2:亮绿*/
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮青 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:亮水 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:黯红  */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:暗水 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	unsigned char table2[216 * 3];
	int r,g,b;
	set_palette(0, 15, table_rgb);
	for(b = 0; b < 6; b++)
		for(g = 0; g < 6; g++)
			for(r = 0; r < 6; r++)
			{
				table2[ (r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[ (r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[ (r + g * 6 + b * 36) * 3 + 2] = b * 51;
			
			}
			
			
	set_palette(16, 231, table2);
	return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/*  */
	io_cli(); 					/* 禁止中断 */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/*  */
	return;
}
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x,y;
	for(y=y0;y<=y1;y++)
		for(x=x0;x<=x1;x++)
			vram[y*xsize+x]=c;
	return;
}


void init_screen(char *vram, int x, int y)
{
	boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	return;
}
void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char*p;
	for(i=0;i<16;i++)
	{
		p = &vram[(y + i)*xsize+ x];
		if((font[i]&0x80) != 0){p[0] = c;}
		if((font[i]&0x40) != 0){p[1] = c;}
		if((font[i]&0x20) != 0){p[2] = c;}
		if((font[i]&0x10) != 0){p[3] = c;}
		if((font[i]&0x08) != 0){p[4] = c;}
		if((font[i]&0x04) != 0){p[5] = c;}
		if((font[i]&0x02) != 0){p[6] = c;}
		if((font[i]&0x01) != 0){p[7] = c;}
		
	}
	return;
}
void putfont8_chinese(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i, j;
	char*p;
	for(i = 0;i < 16;i++)
	{
		p = &vram[(y + i)*xsize+ x];
		if((font[i * 2]&0x80) != 0){p[0] = c;}
		if((font[i * 2]&0x40) != 0){p[1] = c;}
		if((font[i * 2]&0x20) != 0){p[2] = c;}
		if((font[i * 2]&0x10) != 0){p[3] = c;}
		if((font[i * 2]&0x08) != 0){p[4] = c;}
		if((font[i * 2]&0x04) != 0){p[5] = c;}
		if((font[i * 2]&0x02) != 0){p[6] = c;}
		if((font[i * 2]&0x01) != 0){p[7] = c;}
		if((font[i * 2 + 1]&0x80) != 0){p[8] = c;}
		if((font[i * 2 + 1]&0x40) != 0){p[9] = c;}
		if((font[i * 2 + 1]&0x20) != 0){p[10] = c;}
		if((font[i * 2 + 1]&0x10) != 0){p[11] = c;}
		if((font[i * 2 + 1]&0x08) != 0){p[12] = c;}
		if((font[i * 2 + 1]&0x04) != 0){p[13] = c;}
		if((font[i * 2 + 1]&0x02) != 0){p[14] = c;}
		if((font[i * 2 + 1]&0x01) != 0){p[15] = c;}
		
	}
	return;
}
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{

	extern char hankaku[4096];
	struct TASK *task = task_now();
	char *nihongo = (char*)*((int*)0xfe8),*font;
	

	if(task->langmode == 0)
	{
		for(; *s != 0x00; s++)
		{
			putfont8(vram,xsize, x, y, c, hankaku + *s * 16);
			x += 8;
		}
	}
	if (task->langmode == 1)
	{
		for (; *s != 0x00; s++)
		{
			if((unsigned int)*s > 0xa0)
			{
				if(task->langbyte == 0)
				{
					task->langbyte = (unsigned char)*s - 0xa0;
				}else
				{
					font = nihongo + ((task->langbyte - 1) * 94 + (unsigned char)*s - 0xa1) * 32;
					putfont8_chinese(vram, xsize, x - 8, y, c,font);
					task->langbyte = 0;
				}
			}else
			{
				putfont8(vram, xsize, x, y, c, hankaku  + *s * 16);
				
			}
			
			x += 8;
		}
	}
	
}

void init_mouse_cursor8(char *mouse, char bc)
{
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}