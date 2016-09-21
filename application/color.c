extern api_putchar(int c);
extern api_putstr(char * s);
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
void main()
{
	char buf[144 * 164];
	int win, x, y , r, g, b;
	win = api_openwin(buf, 144, 164, -1, "color");
	for(y = 0; y < 128; y++)
		for(x = 0; x < 128; x++)
		{
			r = x * 2;
			g = y * 2;
			b = 0;
			buf[(x + 8) + (y + 28) * 144] = 16 + (r / 43) + (g / 43) * 6 + (b / 43) * 36;
		}
	api_refreshwin(win, 8, 28, 136, 156);
	api_getkey(1);
	return;
}
unsigned char rgb2pal(int r, int g, int b, int x, int y)
{
	static int table[4] = { 3, 1, 0, 2};
	int i;
	x &=1;
	y &=1;
	i = table[x + y *2];
	r = (r * 21) / 256;
	g = (g * 21)/256;
	b = (b*21)/256;
	r = ( r + i)/4;
	g = (g + i ) / 4;
	b = (b + i) / 4;
	return 16 + r + g * 6 + b * 36;
}