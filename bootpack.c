#include "system.h"
extern struct TASKCTL * taskctl;
int cons_newline(int cursor_y, struct SHEET * sheet);

void hello()
{
	struct CONSOLE * cons;
	struct FIFO32 fifo,keycmd;
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	char  fifobuf[128],keycmd_buf[32];
	struct MOUSE_DEC mdec = {0};
	static char windwtitle[] = "HELLOWORLD";
	static char windwtxt[] = "I MISS YOU!";
	static char timertxt[] = "10[sec]";
	static char consoletxt[] = "console";
	static char task_a_name[] = "task_a";
	static char endapp[] = "end appli";
	static char b[]=" ";
		static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	int key_to = 0, key_shift = 0, key_leds = (binfo->leds >> 4)& 7, keycmd_wait = -1, j =0;
	struct TASK *task_cons, *task_a;
	
	struct TIMER *timer;
	char a[2] = {0,0},s[10];
	int mx ,my ,i,cursor_x,cursor_c,task_b_esp;
	unsigned int memtotal, counter=0;
	struct MEMMAN*memman = (struct MEMMAN*)MEMMAN_ADDR;
	
	struct SHTCTL *shtctl;
	struct SHEET* sht_back,*sht_mouse,*sht_win, *sht_cons;
	unsigned char *buf_back,buf_mouse[256],*buf_win,*buf_cons;
    
    gdt_install();
    idt_install();
	init_pic();
	init_pit();
	io_sti();
	
	fifo32_init(&fifo, 128, fifobuf, 0);//初始化队列数据
	fifo32_init(&keycmd, 32, keycmd_buf, 0);
	
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
	
	//sht_back
	sht_back  = sheet_alloc(shtctl);
	buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);//申请初始化屏幕背景内存
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);//设置背景层面的数据
	init_screen(buf_back, binfo->scrnx, binfo->scrny);
	
	//sht_win_b
	
	sht_cons = sheet_alloc(shtctl);
	buf_cons = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
	sheet_setbuf(sht_cons, buf_cons, 256, 165, -1); 
	make_window8(buf_cons, 256, 165, consoletxt, 0);
	make_textbox8(sht_cons, 8, 28, 240, 128, COL8_000000);
	task_cons = task_alloc();
	task_cons->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
	task_cons->tss.eip = (int) &console_task;
	task_cons->tss.es = 1 * 8;
	task_cons->tss.cs = 2 * 8;
	task_cons->tss.ss = 1 * 8;
	task_cons->tss.ds = 1 * 8;
	task_cons->tss.fs = 1 * 8;
	task_cons->tss.gs = 1 * 8;
	*((int *) (task_cons->tss.esp + 4)) = (int) sht_cons;
	*((int *) (task_cons->tss.esp + 8)) = memtotal;
	task_run(task_cons, 2, 2); 

	//sht_win
	sht_win   = sheet_alloc(shtctl);
	buf_win   = (unsigned char *) memman_alloc_4k(memman, 160 * 52);//申请初始化窗口内存
	sheet_setbuf(sht_win,   buf_win,   160,52, -1);
	make_window8(buf_win, 160,52,windwtitle);
	make_textbox8(sht_win, 8, 28, 144, 16, COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_000000;
	//初始化定时器
	timer = timer_alloc();
	timer_init(timer, &fifo, 10);
	timer_settime(timer, 100);
	
	//sht_mouse
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	sheet_slide(sht_back, 0, 0);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	
	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_cons, 32,  4);
	sheet_slide(sht_win,        8,  56);
	sheet_slide(sht_mouse, mx, my);
	//sheet_slide(sht_win, 80, 72);
	sheet_updown(sht_back,  0);
	sheet_updown(sht_cons,  1);

	sheet_updown(sht_win, 4);
	sheet_updown(sht_mouse, 5);
    sheet_refresh(sht_back, 0, 0, binfo->scrnx, 48);
	
	//fifo32_put(&keycmd, KEYCMD_LED);
	//fifo32_put(&keycmd, key_leds);
	
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
			task_sleep(task_a);
			io_sti();
		}else
		{
			i = fifo32_get(&fifo);
			io_sti();
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
				if(a[0] != 0)
				{
				
					if(key_to == 0)
					{
						if(cursor_x < 128)
						{
							a[1] = 0;
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, a, 1);
							cursor_x += 8;
						}
					}else
					{
						fifo32_put(&task_cons->fifo, a[0] + 256);
					}
				}		
					
				if (i == 256 + 0x0e) {//退格键
					if(key_to == 0)
					{
						if(cursor_x > 8)
						{
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, b, 1);
							cursor_x -= 8;
						}
					}else
					{
						fifo32_put(&task_cons->fifo, 8 + 256);
					}
					
				}
				if (i == 256 + 0x0f) /* Tab */
				{ 
					if (key_to == 0) {
						key_to = 1;
						make_wtitle8(buf_win,  sht_win->bxsize,  task_a_name,  0);
						make_wtitle8(buf_cons, sht_cons->bxsize, consoletxt, 1);
						cursor_c = -1;
						boxfill8(buf_win, sht_win->bxsize, COL8_FFFFFF, cursor_x, 28, cursor_x + 7, 43);
						fifo32_put(&task_cons->fifo, 2);
					} else {
						key_to = 0;
						make_wtitle8(buf_win,  sht_win->bxsize,  task_a_name,  1);
						make_wtitle8(buf_cons, sht_cons->bxsize, consoletxt, 0);
						cursor_c = COL8_000000;
						fifo32_put(&task_cons->fifo, 3);
					}
					sheet_refresh(sht_win,  0, 0, sht_win->bxsize,  21);
					sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21);
				}
				if(i == 256 + 0x1c)
				{
					if(key_to != 0)
						fifo32_put(&task_cons->fifo, 10 + 256);
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
				if(i == 256 + 0x3b && key_shift != 0 && task_cons->tss.ss0 != 0)
				{
					cons = (struct CONSOLE*)*((int*)0x0fec);
					cons_putstr(cons, endapp);
					io_cli();
					task_cons->tss.eax = (int)&(task_cons->tss.esp0);
					task_cons->tss.eip = (int) asm_end_app;
					io_sti();
				}
				if(cursor_c >= 0)
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			}
			else if(512 <= i && i <= 767)
			{	
				io_sti();
				if(mousedecode(&mdec, i - 512) == 1)
				{
					mx += mdec.x;
					my += mdec.y;
					mx = (mx >= 0 ? ((mx <= binfo->scrnx - 1) ? mx:binfo->scrnx - 1) : 0);
					my = (my >= 0 ? ((my <= binfo->scrny - 1) ? my:binfo->scrny-1) : 0);
					sheet_slide(sht_mouse, mx, my);
					if(mdec.btn & 0x01 != 0)
					{
						sheet_slide(sht_win,mx-80,my-80);
					}
					
				}	
			}else if(i == 10)
			{
				//putfonts8_asc_sht(sht_back, 0, 64, COL8_FFFFFF, COL8_008484, windwtitle, 10);
				timer_settime(timer,100);
			}
		}
	}
}


