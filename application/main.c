extern api_putchar(int c);
extern api_putstr(char * s);
extern int api_openwin(char * buf, int xis, int ysiz, int col_inv, char *title);
extern void api_putstrwin(int win, int x, int y, int col, int len, char* str);
extern void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
extern void api_point(int win, int x, int y, int col);       extern void api_refreshwin(int win, int x0, int y0, int x1, int y1);
extern void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
extern void api_cloasewin(int win);

void main()
{
	char buf[160 * 100];
	static char string[] = "hello, world";
	static char hello[] = "lines";
	static char star[] = "*";
	int win, i, x, y;
	
	win = api_openwin(buf, 160, 100, -1 , hello);
	api_putstr(hello);
	api_boxfilwin(win, 4, 24, 155, 95, 0);
	x = 76; 
	y = 56;
	api_putstrwin(win, x, y, 3, 1, star);
	for(;;)
	{
		i = api_getkey(1);
		api_putstrwin(win ,x, y, 0, 1, star);
		if( i == '4' && x > 4){x-=8;}
		if(i == '6'  && x < 148){x +=8;}
		if(i == '8' && y > 24){y -= 8;}
		if(i == '2' && y > 24){y += 8;}
		if(i == 0x0a) break;
		api_putstrwin(win, x, y, 3, 1, star);
	}
	api_closewin(win);
	return;
}