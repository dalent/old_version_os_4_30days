//graphic.c 
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void write_byte(int addr,int color );
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

void init_screen(char *vram, int x, int y);

void putfont8(char *vram, int xsize, int x, int y, char c, char *font);

void putfont8_asc(char *vram, int xsize, int x, int y, char c, char *s);

void init_mouse_cursor8(char *mouse, char bc);

void putblock8_8(char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, char *buf, int bxsize);
    
//kernelFunc.asm
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr();
void load_idtr();
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
void asm_cons_putchar(void);
void asm_inthandler0d(void);
void asm_inthandler0c(void);
void asm_hrb_api();
void taskswitch4();
void load_tr(int);
void farjmp(int eip,int cs);
void farcall(int eip,int cs);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app();
//gdt.c
void gdt_install();

//idt.c
void idt_install();

//pic.c
void init_pic(void);
void inthandler21(int *esp);
void inthandler2c(int *esp);
void inthandler27(int *esp);

#define ADR_BOOTINFO	0x00000ff0
//多任务相关
struct gdt_entry
{   unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access_right;
    unsigned char  granularity;
    unsigned char  base_high;
}__attribute__((packed));

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

#define MAX_TASKS 100
#define TASK_GDT0 3
#define MAX_TASKS_LV 10
#define MAX_TASKLEVELS 10
struct TASK;
struct FIFO32
{
	int * buf;
	int p,q,size,free,flags;
	struct TASK * task;
};
struct FILEINFO
{
	unsigned char name[8],ext[3],type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
#define ADR_DISKIMG 0x100000
struct cons;
struct FILEHANDLE
{
	char * buf;
	int size;
	int pos;
};
struct TASK
{
	int sel,flags;//sel 保存gdt编号
	int priority,level;	
	struct CONSOLE*cons;	struct FIFO32 fifo;
	struct TSS32 tss;
	struct gdt_entry ldt[2];
	int ds_base,cons_stack;//保存栈地址以及数据基址
	struct FILEHANDLE* fhandle;
	int *fat;
	char *cmdline;
	unsigned char langmode, langbyte;
};
struct TASKLEVEL
{
	int running;//正在运行的任务数量
	int now ;//正在运行的任务
	struct TASK* tasks[MAX_TASKS_LV];
};
struct TASKCTL
{
	int now_lv;//现在活动中的level
	int lv_change;//在下次任务切换时是否需要改变level
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

struct BOOTINFO{
	char cyls,leds,vmode,reserve;
	short scrnx,scrny;
	char* vram;
};

struct KEYBUF{
	unsigned char data[32];
	int next_r,next_w,len;
	};



struct MOUSE_DEC
{
	unsigned char mouse_dbuf[3],mouse_phase; 
	int x,y,btn;
};

//kb.c
void init_keyboard(struct FIFO32* fifo, int data0);
//mouse.c
void enable_mouse(struct FIFO32* fifo, int data0, struct MOUSE_DEC *mdec);

//fifo.c

void fifo32_init(struct FIFO32 *fifo,int size, int *buf,struct TASK* task);//初始化
int fifo32_put(struct FIFO32*fifo, int data);
int fifo32_get(struct FIFO32*fifo);
int fifo32_status(struct FIFO32*fifo);

//unity
void itoa10(int num, char* des);
void memcpy(char * des, const char * src);
void *strcpy(void *dest, const void *src, unsigned int  count);
void *memset(void *dest, char val, unsigned int  count);
unsigned int  strlen(const char *str);
void sprintf(char * des, const char* format, int num);
int strcmp(const char * first, const char * second);
int strncmp(const char * first, const char * second, unsigned int size);

//memory
#define MEMMAN_FREES 4090
struct FREEINFO
{
	unsigned int addr,size;
};
struct MEMMAN{
	unsigned int frees,maxfrees,lostsize,losts;
	struct FREEINFO free[MEMMAN_FREES];
};
void memman_init(struct MEMMAN*man);
unsigned int memman_total(struct MEMMAN*man);
unsigned int memman_alloc(struct MEMMAN*man,unsigned int size);
int memman_free(struct MEMMAN* man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN*man, unsigned int size);
int memman_free_4k(struct MEMMAN*man,unsigned int addr ,unsigned int size);

//sheet.c
#define MAX_SHEETS 256
struct SHTCTL;
struct SHEET
{
	char * buf;
	int bxsize,bysize,vx0,vy0,col_inv,height ,flags;
	struct SHTCTL*ctl;
    struct TASK*task;
};
struct SHTCTL
{
	unsigned char * vram,*map;
	int xsize,ysize,top;
	struct SHEET*sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
struct SHTCTL * shtctl_init(struct MEMMAN*memman,unsigned char *vram,int xsize,int ysize);
struct SHEET *sheet_alloc(struct SHTCTL*ctl);
void sheet_setbuf(struct SHEET*sht,unsigned char * buf,int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET*sht,int hegiht);
void sheet_refresh( struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide( struct SHEET*sht, int vx0, int vy0);
void sheet_free(struct SHEET*sht);


//timer.c
#define MAX_TIMER 500
void inthandler20(int *esp);
void init_pit(void);
struct TIMER* timer_alloc(void);
void timer_free(struct TIMER*timer);
void timer_settime(struct TIMER*timer, unsigned int timeout);
void timer_init(struct TIMER*timer, struct FIFO32*fifo, unsigned char data);
void timer_cancelall(struct FIFO32*fifo);
struct TIMER
{
	struct TIMER * next;
	unsigned int timeout,flags,flags2;
	struct FIFO32 *fifo;
	int data;
	
};
struct TIMERCTL
{
	unsigned int count ,next;
	struct TIMER timer0[MAX_TIMER];//保存最多的时钟个数 
	struct TIMER*t0;// 我们使用排序有序链表的方式保存时钟链
};

#define COL8_000000		0//黑色
#define COL8_FF0000		1/*  1:亮红*/
#define COL8_00FF00		2/*  2:亮绿*/
#define COL8_FFFF00		3/*  3:亮黄 */
#define COL8_0000FF		4/*  4:亮青 */
#define COL8_FF00FF		5/*  5:亮紫 */
#define COL8_00FFFF		6/*  6:亮水 */
#define COL8_FFFFFF		7/*  7:白 */
#define COL8_C6C6C6		8/*  8:亮灰 */
#define COL8_840000		9/*  9:黯红  */
#define COL8_008400		10/* 10:暗绿 */
#define COL8_848400		11/* 11:暗黄 */
#define COL8_000084		12/* 12:暗青 */
#define COL8_840084		13/* 13:暗紫 */
#define COL8_008484		14/* 14:暗水 */
#define COL8_848484		15/* 15:暗灰 */

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

#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA	0x0064
#define PORT_KEYCMD	0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WAITMODE 0x60
#define KBC_MODE 0x47
//mtask.c

void task_switch(void);
void task_run(struct TASK*task, int level, int priority);
struct TASK* task_alloc(void);
struct TASK* task_init(struct MEMMAN*memman);

void file_readfat(int * fat, unsigned char * img);
void file_loadfile(int clustno, int size, char * buf, int *fat, char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
#define MEMMAN_ADDR 0x3c0000
#define KEYCMD_LED 0xed
struct CONSOLE
{
	struct SHEET*sht;
	int cur_x, cur_y, cur_c;
	struct TIMER*timer;
};
int mousedecode(struct MOUSE_DEC* mdec, unsigned char dat);
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void make_wtitle8(unsigned char * buf, int xsize, char * title, char act);
void change_wtitle8(struct SHEET* sht, char act);
void console_task(struct SHEET *sheet,unsigned int);
int* hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
int* inthandler0d(int *esp);
int* inthandler0c(int*esp);
void cons_putstr(struct CONSOLE*cons, char * s);
struct SHEET* open_console(struct SHTCTL*shtctl, unsigned int memtotal);
struct TASK* open_constask(struct SHEET*sht, unsigned int memtotal);