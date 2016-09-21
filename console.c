#include "system.h"

void cons_newline(struct CONSOLE *cons);
void cons_putchar(struct CONSOLE* cons, int chr, char move);
void cmd_hlt(struct CONSOLE *cons, int *fat);
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);
void cmd_dir(struct CONSOLE* cons);
void cmd_cls(struct CONSOLE* cons);
void cmd_mem(struct CONSOLE* cons, unsigned int memtotal);
void cons_runcmd(char* cmdline, struct CONSOLE* cons, int *fat, unsigned int memtotal);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);
void cmd_exit(struct CONSOLE* cons, int * fat);
void hrb_api_linewin(struct SHEET * sht, int x0, int y0, int x1, int y1, int col);
void cmd_start(struct CONSOLE*cons, char *cmdline, int memtotoal);
void cmd_langmode(struct CONSOLE* cons, char *cmdline);
extern struct TASKCTL * taskctl;
void console_task(struct SHEET *sheet, unsigned int memtotal)
{
	struct TIMER *timer;
	struct TASK *task = task_now();
	int i,   cursor_c = -1;
	char  cmdline[30];
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int *fat = (int *)memman_alloc_4k(memman, 4 * 2880);  //一个磁盘总共2880个扇区，所以只保存2880个int就可以啦
	struct CONSOLE cons;
	struct FILEHANDLE * fhandle[8];
	unsigned char *nihongo = (char*)*((int*)0xfe8);
	
	
	for(i = 0; i < 8 ; i++)
	{
		fhandle[i]->buf = 0;//未使用标记
	}
	task->fhandle =fhandle;
	task->fat = fat;
	cons.sht = sheet;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	task->cons = &cons;
	task->cmdline = cmdline;
	if(sheet != 0)
	{
		timer = timer_alloc();
		timer_init(timer, &task->fifo, 1);
		timer_settime(timer, 50);
	}
	
	if(nihongo[4096] != 0xff)
		task->langmode = 1;
	else 
		task->langmode = 0;
	cons_putchar(&cons, '>', 1);

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) { 
				if (i != 0) {
					timer_init(timer, &task->fifo, 0);
					if(cons.cur_c >= 0)
						cons.cur_c = COL8_FFFFFF;
				} else {
					timer_init(timer, &task->fifo, 1);
					if(cons.cur_c >= 0)
						cons.cur_c = COL8_000000;
				}
				timer_settime(timer, 50);
			}
			if(i == 2)
				cons.cur_c = COL8_FFFFFF;
				
			if(i == 3)
			{
				if(sheet != 0)
				{
					boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				cons.cur_c = -1;
			}
			if( i == 4)
			{
				cmd_exit(&cons, fat);
			}
			if (256 <= i && i <= 511) 
			{ 
				if (i == 8 + 256) 
				{//退格
					if(cons.cur_x > 16)
					{
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
					
				}else if( i == 10 + 256)//回车
				{
					cons_putchar(&cons, ' ', 0);//擦除光标
					cmdline[ cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);//换行
					cons_runcmd(cmdline, &cons, fat, memtotal);//运行命令
					
					if(sheet == 0)
					{
						cmd_exit(&cons, fat);
					}	
					
					cons_putchar(&cons, '>', 1);//显示提示符
				}				
				else//一般字符
				{
					if (cons.cur_x < 240) 
					{
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			if(sheet != 0)
			{
				if(cons.cur_c >= 0)
					boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16; /* 换行 */
	} else {
		/*滚动 */
		if(sheet != 0)
		{
			for (y = 28; y < 28 + 112; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
				}
			}
			for (y = 28 + 112; y < 28 + 128; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = COL8_000000;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
		}
	}
	cons->cur_x = 8;
}

void cons_putchar(struct CONSOLE* cons, int chr, char move)
{
	static char space[] = " ";
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if(s[0] == 0x09)
	{
		for(;;)
		{
			if(cons->sht != 0)
			{
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, space, 1);
			}
			cons->cur_x += 8;
			if(cons->cur_x == 8 + 240)
				cons_newline(cons);
			if(((cons->cur_x - 8) & 0x1f) == 0)
			{
				break;
			}
		}
	}else if(s[0] == 0x0a)//换行
	{
		cons_newline(cons);
	}else if(s[0] == 0x0d)//回车
	{
		
	}else
	{
		if(cons->sht != 0)
		{
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		}
		if(move != 0)
		{
			cons->cur_x += 8;
			if(cons->cur_x == 8 + 240)
				cons_newline(cons);
		}

	}
}

void cons_runcmd(char* cmdline, struct CONSOLE* cons, int *fat, unsigned int memtotal)
{
	static char badcommand[] = "Bad command.\n\n";
	static char mem[] = "mem";
	static char cls[] = "cls";
	static char dir[] = "dir";
	static char type[] = "type ";
	static char close[] = "exit";
	static char start[] = "start ";
	static char ncst[] = "ncst ";
	static char langmode[] = "langmode " ;
	static char hlt[] = "hlt";
	if(strcmp(cmdline, mem) == 0 && cons->sht != 0)
	{
		cmd_mem(cons, memtotal);
	}else if(strcmp(cmdline, cls) == 0 && cons->sht != 0)
	{
		cmd_cls(cons);
	}else if(strcmp(cmdline, dir) == 0 && cons->sht != 0)
	{
		cmd_dir(cons);
	}
	//else if(strncmp(cmdline, type, 5) == 0 && cons->sht != 0)
	//{
		//cmd_type(cons, fat, cmdline);
	//}
	else if(strcmp(cmdline, close) == 0)
	{
		cmd_exit(cons, fat);
	}else if(strncmp(cmdline, start, 6) == 0)
	{
		cmd_start(cons, cmdline, memtotal);
	}else if(strncmp(cmdline, ncst, 5) == 0)
	{	
		cmd_ncst(cons, cmdline , memtotal);
	}else if(strncmp(cmdline, langmode, 9) == 0)
	{
		cmd_langmode(cons, cmdline);
	}
	else if(cmdline[0] != 0)
	{
		if(cmd_app(cons, fat, cmdline) == 0)
		{
			cons_putstr(cons, badcommand);
		}
	
	}
	
}
void cmd_langmode(struct CONSOLE* cons, char *cmdline)
{
	static char modemsg[] =  "mode number error \n";
	struct TASK *task = task_now();
	
	unsigned char mode = cmdline[9] - '0';
	if(mode <= 1)
		task->langmode = mode;
	else
		cons_putstr(cons, modemsg);
		
	cons_newline(cons);
	return;
	
}
void cmd_ncst(struct CONSOLE*cons, char *cmdline, int memtotal)
{
	struct TASK *task = open_constask(0 ,memtotal);
	struct FIFO32 * fifo = &task->fifo;
	int i;
	for(i = 5; cmdline[i] != 0; i++)
	{
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);
	cons_newline(cons);
	return;
}
void cmd_start(struct CONSOLE*cons, char *cmdline, int memtotal)
{
	struct SHTCTL*shtctl = (struct SHTCTL*)*((int *)0xfe4);
	struct SHEET*sht = open_console(shtctl, memtotal);
	struct FIFO32*fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	for(i = 6; cmdline[i] != 0; i++)
	{
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);
	cons_newline(cons);
	return;
	
}
void cmd_mem(struct CONSOLE* cons, unsigned int memtotal)
{
	static char total[] = "total   %dMB\n";
	static char free[] = "free %dKB\n";
	struct MEMMAN *memman = (struct MEMMAN*)MEMMAN_ADDR;
	char s[30];
	sprintf(s, total, memtotal / (1024 * 1024));
	cons_putstr(cons, s);
	sprintf(s, free, memman_total(memman) / 1024);
	cons_putstr(cons, s);
	cons_newline(cons);	
	
}
void cmd_cls(struct CONSOLE* cons)
{
	int x,y;
	struct SHEET *sheet = cons->sht;
	for(y = 28; y < 28 + 128; y++)
		for(x = 8; x < 8 + 240; x++)
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
}
void cmd_dir(struct CONSOLE* cons)
{
	static char file[] = "filename.ext  %d\n";
	struct FILEINFO*finfo = (struct FILEINFO*)(ADR_DISKIMG + 0x2600);
	int x,y;
	char s[30];
	for(x = 0; x < 224; x++)
	{
		if(finfo[x].name[0] == 0x00)
			break;
		
		if(finfo[x].name[0] != 0x5e)
		{
			if((finfo[x].type & 0x18) == 0)
			{
				sprintf(s, file, finfo[x].size);
				for(y = 0; y < 8; y++)
					s[y] = finfo[x].name[y];
			}
			s[9] = finfo[x].ext[0];
			s[10] = finfo[x].ext[1];
			s[11] = finfo[x].ext[2];
			cons_putstr(cons, s);
		}
		
	}
	cons_newline(cons);
	return;
}
/*void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline)
{
	static char notfound[] = "file not found.\n";
	struct MEMMAN *memman = (struct MEMMAN*)MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO*)(ADR_DISKIMG + 0x2600), 224);
	char* p;
	int i;
	if(finfo != 0)
	{
	
		p = (char* )memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*) (ADR_DISKIMG + 0x003e00));
		
		for(i = 0; i < finfo->size; i++)
		{
			cons_putchar(cons, p[i], 1);
		}
		memman_free_4k(memman, (int)p, finfo->size);
	}else//没有找到文件
	{
		cons_putstr(cons, notfound);
	}
	cons_newline(cons);
	return;
}
*/
void cmd_hlt(struct CONSOLE *cons, int *fat)
{
	static char notfound[] = "file not found.\n";
	static char hlt_hrs[] = "HLT.HRB"; 
	struct MEMMAN *memman = (struct MEMMAN*)MEMMAN_ADDR;
	char *p;
	struct FILEINFO *finfo = file_search(hlt_hrs, (struct FILEINFO*)(ADR_DISKIMG + 0x2600), 224);
	if(finfo != 0)
	{
	
		p = (char* )memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*) (ADR_DISKIMG + 0x003e00));
		gdt_set_gate(103, (int)p, finfo->size - 1, 0x9a, 0x40);
		farjmp(0, 103 * 8);
		memman_free_4k(memman, (int)p, finfo->size);
	}else//没有找到文件
	{
		cons_putstr(cons, notfound);
	}
	cons_newline(cons);
	return;

}
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN*)MEMMAN_ADDR;
	struct FILEINFO*finfo;
	char name[18], *p, *q;
    struct SHEET*sht;
    struct SHTCTL*shtctl;
	int i;
	struct TASK*task = task_now();
	for(i = 0; i < 13; i++)
	{
		if(cmdline[i] <= ' ')
			break;
		name[i] = cmdline[i];
	}
	
	name[i] = 0;//到此为止
	finfo = file_search(name, (struct FILEINFO*)(ADR_DISKIMG + 0x2600), 224);
	if(finfo == 0&& name[i - 1] != '.')
	{
		name[i] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO*)(ADR_DISKIMG + 0x2600), 224);
	}
	if(finfo != 0)
	{
		p = (char* )memman_alloc_4k(memman, finfo->size);
		q = (char* )memman_alloc_4k(memman, 64 * 1024);
		task->ds_base = (int)q;
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*) (ADR_DISKIMG + 0x003e00));
		ldt_set_gate(task->ldt + 0, (int)p, finfo->size - 1, 0x9a + 0x60, 0x40);
		ldt_set_gate(task->ldt + 1, (int)q, 64 * 1024 - 1, 0x92 + 0x60, 0x40);
		for(i =0; i < finfo->size; i++)
			q[i] = p[i];
		start_app(0, 0 * 8 + 4, 64 * 1024, 1 * 8 + 4, &(task->tss.esp0));
        shtctl = (struct SHTCTL*)*((int*)0xfe4);
        for(i = 0; i< MAX_SHEETS; i ++)
        {
            sht = &(shtctl->sheets0[i]);
            if((sht->flags & 0x11)== 0x11 && sht->task == task)
            {
                sheet_free(sht);
            }
        }
		
		timer_cancel(&task->fifo);
		for(i = 0; i < 8; i++)
		{
			if(task->fhandle[i].buf != 0)
			{
				memman_free_4k(memman, (int)task->fhandle[i].buf, task->fhandle[i].size);
				task->fhandle[i].buf = 0;
			}
		}
		memman_free_4k(memman, (int)p, finfo->size);
		memman_free_4k(memman, (int)q, 64 * 1024);
		cons_newline(cons);
		return 1;
		
	}
	return 0;
}
void cons_putstr(struct CONSOLE*cons, char * s)
{
	for(; *s != 0; s++)
	{
		cons_putchar(cons, *s, 1);
	}	
}
void cons_putstrn(struct CONSOLE*cons, char *s, int l)
{
	int i;
	for(i = 0; i < l; i++)
	{
		cons_putchar(cons, s[i], 1);
	}
}

//给应用程序提供的api
int* hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK*task = task_now();
	struct CONSOLE* cons = task->cons;
	
	char s[20];
	static char error[] = "%d error\n";
	static char errorip[] = "ebx = %d \n";
	int ds_base = task->ds_base;
	struct FILEHANDLE *fh;
	struct FILEINFO* finfo;
	struct FIFO32* sys_fifo = (struct FIFO32*)*((int*)0x0fec);
	struct SHTCTL*shtctl = (struct SHTCTL*)*(int*)0x0fe4;
	struct MEMMAN*memman = (struct MEMMAN*) MEMMAN_ADDR;
	struct SHEET* sht;
	int *reg = &eax + 1, i;//强行改写通过pushad保存的值
	if(edx == 1)
	{
		cons_putchar(cons, eax & 0xff, 1);
	}else if(edx == 2)
	{
		cons_putstr(cons, (char*)ebx + ds_base);
	}else if(edx == 3)
	{
		cons_putstrn(cons, (char*)ebx + ds_base, ecx); 
	}else if(edx == 4)
	{
		return &(task->tss.esp0);
	}else if(edx == 5)
	{
		sht = sheet_alloc(shtctl);
        sht->task = task;
        sht->flags |= 0x10;
		sheet_setbuf(sht, (char*)ebx + ds_base, esi, edi ,eax);
		make_window8((char*)ebx + ds_base, esi, edi, (char*)ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) /2) & ~3, (shtctl->ysize - edi)/2);
		sheet_updown(sht, shtctl->top);
		reg[7] = (int)sht;
	}else if(edx == 6)
	{
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi ,eax, (char*)ebp + ds_base);
		if((ebx & 1) == 0)
			sheet_refresh(sht, esi ,edi ,esi + ecx * 8, edi + 16);
	}else if( edx == 7)
	{
		sht = (struct SHEET *)(ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if((ebx & 1) == 0)
			sheet_refresh(sht, eax, ecx, esi + 1 , edi + 1);
	}else if( edx == 11)
	{
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if((ebx & 1) == 0)
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
	}else if(edx == 12)
	{
		sht = (struct SHEET*) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	}else if(edx == 13)
	{
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		hrb_api_linewin(sht, eax, ecx, esi ,edi , ebp);
		//cons_putstr(cons, s);
		if((ebx & 1) == 0 )
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		
	}else if (edx == 14)
	{
		sheet_free((struct SHEET*) ebx);
	}else if( edx == 15)
	{
		for(;;)
		{
			io_cli();
			if(fifo32_status(&task->fifo) == 0)
			{
				if(eax != 0)
				{
					task_sleep(task);
				}else
				{
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}

			i = fifo32_get(&task->fifo);
			io_sti();
			if(i <= 1)
			{
				timer_settime(cons->timer, 50);
			}
			if( i == 2)
			{
				cons->cur_c = COL8_FFFFFF;
			}
			if(i == 3)
			{
				cons->cur_c = -1;
			}
			if(i == 4)
			{
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);
			}
			if( i == 10)
			{
				reg[7] = i;
				return 0;
			}
			if(256 <= i)
			{
               
				reg[7] = i - 256;
               
				return 0;
			}
        }
    }else if(edx == 16)
    {
        reg[7] = (int)timer_alloc();
		((struct TIMER*)reg[7])->flags2 = 1;
    }else if(edx == 17)
    {
        timer_init((struct TIMER*)ebx, &task->fifo, eax);
    }else if (edx == 18)
    {
        timer_settime((struct TIMER*)ebx, eax);
    }else if( edx == 19)
    {
        timer_free((struct TIMER*)ebx);
    }else if(edx == 20)
	{
		if(eax == 0)
		{
			i = io_in8(0x61);
			io_out8(0x61, i & 0xd);
		}else
		{
			i = 1193180000 / eax;
			io_out8(0x43, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03)&0xf);
		}
		return 0;
	}else if(edx == 21)
	{
		for(i = 0; i < 8; i++)
		{
			if(task->fhandle[i].buf == 0)
				break;
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if(i < 8)
		{
			finfo = file_search((char*)ebx + ds_base, (struct FILEINFO*)(ADR_DISKIMG + 0x002600), 224);
			
			if(finfo != 0)
			{
				cons_putstr(cons,(char*)ebx + ds_base);
				reg[7] = (int)fh;
				fh->buf = (char*)memman_alloc_4k(memman, finfo->size);
				fh->size = finfo->size;
				fh->pos = 0;
				file_loadfile(finfo->clustno, finfo->size, fh->buf, task->fat, (char*)(ADR_DISKIMG + 0x3e00));
			}
		}
	}else if(edx == 22)
	{
		fh = (struct FILEHANDLE*)eax;
		memman_free_4k(memman, (int)fh->buf, fh->size);
		fh->buf = 0; 
	}else if(edx == 23)
	{
		fh = (struct FILEHANDLE*) eax;
		if(ecx == 0)
		{
			fh->pos = ebx;
		}else if( ecx == 1)
		{
			fh->pos +=ebx;
		}else if(ecx == 2)
		{
			fh->pos = fh->size + ebx;
		}
		if( fh->pos < 0)
		{
			fh->pos = 0;
		}
		if(fh->pos > fh->size)
		{
			fh->pos = fh->size;
		}
	}else if( edx == 24)
	{
		fh = (struct FILEHANDLE*)eax;
		if(ecx == 0)
		{
			reg[7] = fh->size;
		}else if(ecx == 1)
		{
			reg[7] = fh->pos;
		}else if(ecx == 2)
		{
			reg[7] = fh->pos - fh->size;
		}
	}else if(edx == 25)
	{
		fh = (struct FILEHANDLE*)eax;
		for(i = 0; i < ecx; i++)
		{
			if(fh->pos >= fh->size)
				break;
			*((char*)ebx + ds_base + i) = fh->buf[fh->pos++];
		}
		reg[7] = i;
	}else if(edx == 26)
	{
		i = 0;
		for(;;)
		{
			*((char *)ebx + ds_base + i) = task->cmdline[i];
			if(task->cmdline[i] == 0)
			{
				break;
			}
			
			if(i >= ecx)
			{
				break;
			}
			i++;
		}
		reg[7] = i;
	}
	return 0;
	
}

//画线
void hrb_api_linewin(struct SHEET * sht, int x0, int y0, int x1, int y1, int col)
{
	int x, y, len, dx, dy, i;
	dx = x1 - x0;
	dy = y1 - y0;
	if(dx < 0)
		dx = -dx;
	if(dy < 0)
		dy = -dy;
	x = x0 << 10;
	y = y0 << 10;
	if(dx >= dy)
	{
		len = dx + 1;
		if(x0 > x1)
		{
			dx = -1024;
		}else
		{
			dx = 1024;
		}
		if( y0 <= y1)
		{
			dy = ((y1 - y0 + 1) << 10)/len;
		}
		else 
		{
			dy = ((y0 - y1 + 1) << 10) /len;
		}
	}else
	{
		len = dy + 1;
		if(y0 > y1)
		{
			dy = - 1024;
		}else
		{
			dy = 1024;
		}
		if( x0 <= x1)
		{
			 dx = ((x1 - x0 + 1) << 10) / len;
		}else
		{
			 dx = ((x0 - x1 + 1) << 10) / len;
		}
	}
	
	for(i = 0; i < len; i++)
	{
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}
	
}

//内存保护异常
int* inthandler0d(int *esp)
{
	static char error[] = "\n INT 0D:\n General Protected Exception. \n";
	static char errorip[] = "eip = %d \n";
	struct TASK*task = task_now();
	struct CONSOLE*cons = task->cons;
	char s[30];
	sprintf(s, errorip, esp[11]);
	cons_putstr(cons, error);
	cons_putstr(cons, s);
	return &(task->tss.esp0);
}
//栈异常
int* inthandler0c(int *esp)
{
	struct TASK*task = task_now();
	static char error[] = "\nINT 0C:\n Stack Exception.\n";
	struct CONSOLE*cons = task->cons;
	
	static char errorip[] = "eip = %d \n";
	char s[30];
	sprintf(s, errorip, esp[11]);
	cons_putstr(cons, error);
	cons_putstr(cons, s);
	return &(task->tss.esp0);
}

//退出控制台
void cmd_exit(struct CONSOLE* cons, int * fat)
{
	struct MEMMAN *memman = (struct MEMMAN*)MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL*)*((int*)0xfe4);
	struct FIFO32* fifo = (struct FIFO32*)*((int*)0xfec);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	memman_free_4k(memman, (int)fat, 4*2880);
	io_cli();
	if(cons->sht != 0)
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);
	else
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);
	io_sti();
	for(;;)
		task_sleep(task);
	
}