
extern int api_openwin(char * buf, int xis, int ysiz, int col_inv, char *title);
extern void api_putstrwin(int win, int x, int y, int col, int len, char* str);
extern void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
extern void api_point(int win, int x, int y, int col);       
extern void api_refreshwin(int win, int x0, int y0, int x1, int y1);
extern void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
extern void api_cloasewin(int win);
extern void api_aloctimer();
extern void api_inittimer(int timer, int data);
extern void api_settimer(int timer, int time);
extern void free_timer(int timer);
extern int api_cmdline(char *buf, int maxsize);
extern  int api_fopen(char * fname);
extern void api_fclose(int fhandle);
extern void api_fseek(int fhandle, int offset, int mode);
extern int api_fsize(int fhandle, int mode);
extern int  api_fread(char *buf, int maxsize, int fhandle);
extern void api_putchar(int c);
extern void api_putstr(char * s);
int api_getlang(void);
void api_end();
void api_initmalloc(void);//没有实现，现在应用程序只能在分配给她的栈里面实现