extern api_putchar(int c);
extern api_putstr(char * s);
extern int api_openwin(char * buf, int xis, int ysiz, int col_inv, char *title);
extern void api_putstrwin(int win, int x, int y, int col, int len, char* str);
extern void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
extern void api_point(int win, int x, int y, int col);       extern void api_refreshwin(int win, int x0, int y0, int x1, int y1);
extern void api_linewin(int win, int x0, int y0, int x1, int y1, int col);

void main()
{
	char buf[160 * 100];
	static char string[] = "hello, world";
	static char hello[] = "lines";
	//api_putchar('a');
	int win, i, x, y;
	api_putstr(string);
	win = api_openwin(buf, 160, 100, -1 , hello);
	for(i = 0; i < 8; i++)
	{
		api_linewin( win + 1, 8, 26, 77, i * 9 + 26, i);
		api_linewin(win  +1 , 88, 26, i * 9 + 88, 89, i);
	}
	
	api_refreshwin(win, 6, 26, 154, 90);
	return;
}