#include "system.h"
extern struct TASKCTL * taskctl;
int cons_newline(int cursor_y, struct SHEET * sheet);
void keywin_off(struct SHEET* key_win);
void keywin_on(struct SHEET* key_win);
struct SHEET* open_console(struct SHTCTL*shtctl, unsigned int memtotal);
void close_constask(struct TASK*task);
void close_console(struct SHEET*sht);
void hello()
{
	struct FIFO32 fifo,keycmd;
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	int  fifobuf[128],keycmd_buf[32];
	struct MOUSE_DEC mdec = {0};
	
	char s[20];
	static char pr[] = "ss: %d\n";
	static char endapp[] = "end appli";
    static char closetxt[] = "\nbreak(mouse):\n";
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	
	static char nihongotxt[]  = "NIHONGO.FNT";
	int  key_shift = 0, key_leds = (binfo->leds >> 4)& 7, j =0,x,y,mmx = -1, mmy = -1, mmx2 = 0;
	
	
	//struct TIMER *timer;
	char a[2] = {0,0};
	int mx ,my ,i,new_mx = -1, new_my = 0,new_wx = 0x7fffffff,new_wy = 0;
	unsigned int memtotal;
	struct MEMMAN*memman = (struct MEMMAN*)MEMMAN_ADDR;
	
	struct SHTCTL *shtctl;
	struct SHEET* sht_back,*sht_mouse,*sht = 0, *key_win, *sht2;
	unsigned char *buf_back,buf_mouse[256];
    struct TASK  *task_a,*task;
	//汉字相关
	int *fat;
	unsigned char* nihongo;
	struct FILEINFO *finfo;
	extern char hankaku[4096];
	
    gdt_install();
    idt_install();
	init_pic();
	init_pit();
	io_sti();
	
	fifo32_init(&fifo, 128, fifobuf, 0);//初始化队列数据
	fifo32_init(&keycmd, 32, keycmd_buf, 0);
	
	*((int*)0xfec) = (int)&fifo;
	init_keyboard(&fifo,256);//
	enable_mouse(&fifo, 512, &mdec);//开启鼠标中断
	
	io_out8(PIC0_IMR, 0xf8);//键盘中断许可
    io_out8(PIC1_IMR, 0xef);//鼠标许可
	 
	memtotal = memtest(0x00400000,0xbfffffff);
	memman_init(memtotal);
	memman_free(memman,0x1000,0x0009e000);
	memman_free(memman,0x400000,memtotal - 0x400000);
   
	init_palette();//初始化调色板
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	
	*((int*)0x0fe4) = (int) shtctl;
	//task_a 
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	task_a->langmode = 0;
	
	//sht_back
	sht_back  = sheet_alloc(shtctl);
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);//申请初始化屏幕背景内存
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);//设置背景层面的数据
	init_screen(buf_back, binfo->scrnx, binfo->scrny);
	
	//sht_cons
	key_win = open_console(shtctl, memtotal);
	
	
	
	//初始化定时器
	//timer = timer_alloc();
	//timer_init(timer, &fifo, 10);
	//timer_settime(timer, 100);
	
	//sht_mouse
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	sheet_slide(sht_back, 0, 0);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	
	sheet_slide(sht_back, 0, 0);
	sheet_slide(key_win, 56,  6);
	sheet_slide(sht_mouse, mx, my);
	//sheet_slide(sht_win, 80, 72);
	sheet_updown(sht_back,  0);
	sheet_updown(key_win,  1);
	//sheet_updown(sht_win, 3);
	sheet_updown(sht_mouse, 2);
    sheet_refresh(sht_back, 0, 0, binfo->scrnx, 48);
	
 
	keywin_on(key_win);
    
	//fifo32_put(&keycmd, KEYCMD_LED);
	//fifo32_put(&keycmd, key_leds);
	nihongo = (unsigned char*)memman_alloc_4k(memman, 6763 * 32);
	finfo = file_search(nihongotxt, (struct FILEINFO*)(ADR_DISKIMG + 0x2600), 224);
	if(finfo != 0)
	{
		file_loadfile(finfo->clustno, finfo->size, nihongo, fat, (char*)(ADR_DISKIMG + 0x3e00));
	}
	else
	{
		for(i = 0; i < 6763 * 32; i++)
		{
			nihongo[i] = 0xff;
		}
	}
	*((int*)0xfe8) = (int)nihongo;
	//memman_free_4k(memman, (int)
	for(;;)
	{
		//if(fifo32_status(&keycmd) > 0 && keycmd_wait < 0)
	//	{
			//keycmd_wait = fifo32_get(&keycmd);
			//wait_KBC_sendready();
			//io_out8(PORT_KEYDAT, keycmd_wait);
		//}
		io_cli();
		if(fifo32_status(&fifo) == 0)
		{
			if(new_mx >= 0)
			{
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			}else if(new_wx != 0x7fffffff)
			{
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			}else
			{
				task_sleep(task_a);
				io_sti();
			}
		}else
		{
			i = fifo32_get(&fifo);
			io_sti();
            
			
			if(key_win != 0 && key_win->flags == 0)//窗口被关闭
			{
				if(shtctl->top == 1)
				{
					key_win = 0;
				}else
				{
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
				}
				
			}
			
			if(256 <= i && i <= 511)
			{
				if(i < 256 + 0x80)
				{
					if( key_shift == 0)
					{
						a[0] = keytable0[i - 256];
					}else
					{
						a[0] = keytable1[i - 256];
					}
				}else
				{
					a[0] = 0;
				}
				if('A' <= a[0] && a[0] <= 'Z')
				{
					if((key_leds & 4) == 0 && key_shift == 0 || (key_leds & 4) != 0 && key_shift != 0)
					{
						a[0] += 0x20;
					}
				}
				if(a[0] != 0 && key_win != 0)
				{
				
					fifo32_put(&key_win->task->fifo, a[0] + 256);
					
				}		
				if (i == 256 + 0x0f  && key_win != 0) /* Tab */
				{ 
                    keywin_off(key_win);
                    j = key_win->height - 1;
                    if(j == 0)
                        j = shtctl->top - 1;
                    key_win = shtctl->sheets[j];
                    keywin_on(key_win);
				}
				if(i == 256 + 0x2a)//左shift
				{
					key_shift |= 1;
				}
				if( i == 256 + 0x36)//右shift
				{
					key_shift |= 2;
				}
				if(i == 256 + 0xaa)
				{
					key_shift &= ~1;
				}
				if(i == 256 + 0xb6)
				{
					 key_shift &= ~2;
				}
				if(i == 256 + 0x3a)
				{
					//itoa10(j++, s);
					//putfonts8_asc_sht(sht_back, 0, 40, COL8_FFFFFF, COL8_008484, s, 10);
					if(j++%2 == 0)//估计是操作系统的问题，按一次他发两次消息过来
						key_leds ^= 4;
					//fifo32_put(&keycmd, KEYCMD_LED);//操作系统帮我们处理啦
					//fifo32_put(&keycmd, key_leds);
				}
				if(i == 256 + 0x45)
				{
					key_leds ^= 2;
					//fifo32_put(&keycmd, KEYCMD_LED);
					//fifo32_put(&keycmd, key_leds);
				}
				if(i == 256 + 0x46)
				{
					key_leds ^= 1;
					//fifo32_put(&keycmd, KEYCMD_LED);
					//fifo32_put(&keycmd, key_leds);
				}
				if(i == 256 + 0xfa)
				{
					
					//keycmd_wait = -1 ;
				}
				if(i == 256 + 0xfe)
				{
					
					//wait_KBC_sendready();
					//io_out8(PORT_KEYDAT, keycmd_wait);
				}
				if(i == 256 + 0x3b && key_shift != 0  && key_win != 0)//shift + F1
				{
					task = key_win->task;
					if(task != 0 && task->tss.ss0 != 0)
					{
						cons_putstr(task->cons, endapp);
						io_cli();
						task->tss.eax = (int)&(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);
					}
				
				}
                if(i == 256 + 0x57 && shtctl->top > 2)//F11上反窗口
                {
                    sheet_updown(shtctl->sheets[1], shtctl->top - 1);
                }
				if(i==256 + 0x3c && key_shift != 0)//F2 + shift open new console
				{
					if(key_win != 0)
					{
						keywin_off(key_win);
					}
					
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win,shtctl->top);
					keywin_on(key_win);
				}
   
			
			}
			else if(512 <= i && i <= 767)
			{	
				if(mousedecode(&mdec, i - 512) == 1)
				{
					mx += mdec.x;
					my += mdec.y;
					mx = (mx >= 0 ? ((mx <= binfo->scrnx - 1) ? mx:binfo->scrnx - 1) : 0);
					my = (my >= 0 ? ((my <= binfo->scrny - 1) ? my:binfo->scrny-1) : 0);
					new_mx = mx;
					new_my = my;
					if(mdec.btn & 0x01 != 0)//按下鼠标左键
					{
						if(mmx < 0)
						{
							for(j = shtctl->top - 1; j > 0; j--)
							{
						   
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								//从上到下寻找点击的图层
								if(0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize)
								{
									if(sht->buf[y * sht->bxsize + x] != sht->col_inv)
									{
										
										sheet_updown(sht, shtctl->top - 1 );
										if(sht != key_win)
										{
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										if(3 <= x && x < sht->bxsize - 3 && 3 < y && y <  21)//窗口移动模式
										{
											mmx = mx;
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
										}
										if(sht->bxsize - 21 <= x && x <sht->bxsize - 5 && 5 <= y && y < 19)//关闭按钮
										{
											if( (sht->flags & 0x10) != 0)//应用程序？
											{
												task = sht->task;
												cons_putstr(task->cons, closetxt);
												io_cli();
												task->tss.eax = (int)&(task->tss.esp0);
												task->tss.eip = (int)asm_end_app;
												io_sti();
												task_run(task, -1, 0);
											}else//命令行窗口
											{
												task = sht->task;
												sheet_updown(sht, -1);
												keywin_off(key_win);
												key_win = shtctl->sheets[shtctl->top - 1];
												keywin_on(key_win);
												io_cli();
												fifo32_put(&task->fifo, 4);
												io_sti();
											}
										}
										break;
									}
								}
							}
						}else
						{
							x = mx - mmx;
							y = my - mmy;
							new_wx = (mmx2 + x + 2)& ~3;
							new_wy = new_wy  + y;
							mmy = my;
						   
						}
							
						
					}else
                    {
                        mmx = -1;//没有按下 左键，不移动窗口
						if(new_wx != 0x7fffffff)
						{
							sheet_slide(sht, new_wx, new_wy);
							new_wx = 0x7fffffff;
						}
                    }
					
				}	
			}else if(768 <= i && i <= 1023)
			{
				close_console(shtctl->sheets0 + ( i - 768));
			} else if (1024 <= i && i <= 2023) {
				close_constask(taskctl->tasks0 + (i - 1024));
			}else if( 2024 <= i && i <= 2279)
			{
				sht2 = shtctl->sheets0 + ( i - 2024);
				memman_free_4k(memman, (int)sht2->buf, 256 * 165);
				sheet_free(sht2);
			}
		}
	}
}
void keywin_off(struct SHEET* key_win)
{
    change_wtitle8(key_win, 0);

    if((key_win->flags & 0x20) != 0)
        fifo32_put(&key_win->task->fifo, 3);
 
   
}
void keywin_on(struct SHEET*key_win)
{
    change_wtitle8(key_win, 1);

	if((key_win->flags & 0x20)!= 0)
	{
		fifo32_put(&key_win->task->fifo, 2);
	
	}
    
}
//开启新的console
struct SHEET* open_console(struct SHTCTL*shtctl, unsigned int memtotal)
{
	static char consoletxt[] = "console";
	struct MEMMAN*memman = (struct MEMMAN*)MEMMAN_ADDR;
	struct SHEET * sht_cons =sheet_alloc(shtctl);
	unsigned char * buf_cons = (unsigned char *)memman_alloc_4k(memman, 256 * 165);
	sheet_setbuf(sht_cons, buf_cons, 256, 165, -1); 
	make_window8(buf_cons, 256, 165, consoletxt, 0);
	make_textbox8(sht_cons, 8, 28, 240, 128, COL8_000000);

	sht_cons->task = open_constask(sht_cons, memtotal);
	sht_cons->flags |= 0x20;//应用程序用
	return sht_cons;
}
//关闭窗口对应的task
void close_constask(struct TASK*task)
{
	struct MEMMAN*memman = (struct MEMMAN*)MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int)task->fifo.buf, 128 * 4);
	task->flags = 0;
	return;
}
//关闭窗口
void close_console(struct SHEET*sht)
{
	struct MEMMAN*memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK*task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165);
	sheet_free(sht);
	close_constask(task);
	return;
}

struct TASK* open_constask(struct SHEET*sht, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2); /* level=2, priority=2 */
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}
