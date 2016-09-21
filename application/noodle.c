extern api_putchar(int c);
extern api_putstr(char * s);
extern int api_openwin(char * buf, int xis, int ysiz, int col_inv, char *title);
extern void api_putstrwin(int win, int x, int y, int col, int len, char* str);
extern void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
extern void api_point(int win, int x, int y, int col);       
extern void api_refreshwin(int win, int x0, int y0, int x1, int y1);
extern void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
extern void api_cloasewin(int win);
extern int api_alloctimer();
extern void api_inittimer(int timer, int data);
extern void api_settimer(int timer, int time);
extern void free_timer(int timer);
void main()
{
    char buf[150* 50],s[12] ;
    int win,timer = 0,sec = 0,min =0,hou=0;
    static char noodle[] ="noodle";
    static char dd[] = "timer %d";
    win = api_openwin(buf, 150, 50, -1, noodle);
    timer = api_alloctimer();
    api_inittimer(timer, 128);
    for(;;)
    {
        sprintf(s, dd, timer);
        api_boxfilwin(win, 28, 27,115, 41,7);
        api_putstrwin(win, 28,27, 0, 22, s);
        api_settimer(timer, 100);
        if(api_getkey(1) != 128)
        {
            break;
        }
        sec++;
      
    }
    
}