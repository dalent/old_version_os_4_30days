#include "system.h"
#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040
#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2
struct TIMERCTL timerctl;
extern struct TIMER*task_timer;
void init_pit(void)
{
	int i;
	struct TIMER * t;
	io_out8(PIT_CTRL, 0x34);//控制信息
	io_out8(PIT_CNT0, 0x9c);//设置时钟频率
	io_out8(PIT_CNT0, 0x2e);
	timerctl.next = 0xffffffff;
	for(i = 0;  i < MAX_TIMER; i++)
	{
		timerctl.timer0[i].flags = 0;
	}
	//设置一个哨兵，可以减少比较和插入的代码数
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;
	return ;
}

void inthandler20(int *esp)
{
	char ts = 0;
	struct TIMER* timer;
	io_out8(PIC0_OCW2, 0x60);
	timerctl.count++;//时钟滴答数
	if(timerctl.next > timerctl.count)
		return;
	timer = timerctl.t0;
	for(;;)//寻找超时的时钟
	{
		if(timer->timeout > timerctl.count)
			break;
		timer->flags = TIMER_FLAGS_ALLOC;
		
		if(timer != task_timer)//任务切换计时器
			fifo32_put(timer->fifo, timer->data);
		else 	
			ts = 1;
			
		timer = timer->next;
	}
	
	timerctl.t0 = timer;
	timerctl.next = timerctl.t0->timeout;
	if(ts != 0)
		task_switch();
	
	return;
}

struct TIMER* timer_alloc(void)//寻找可用的时钟
{
	int i;
	for(i = 0; i < MAX_TIMER; i++)
	{
		if(timerctl.timer0[i].flags == 0)
		{
			timerctl.timer0[i].flags = TIMER_FLAGS_ALLOC;
			timerctl.timer0[i].flags2 = 0;
			return &timerctl.timer0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER*timer)
{
	timer->flags = 0;
	return;
}

void timer_init(struct TIMER*timer, struct FIFO32*fifo, unsigned char data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER*timer, unsigned int timeout)//设置时钟 ，加入到时钟链
{
	int e;
	struct TIMER*t,*s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	t = timerctl.t0;
	if(timer->timeout <= t->timeout)
	{
		timerctl.t0 = timer;
		timer->next = t;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	
	for(;;)
	{
		s = t;
		t= t->next;
		if(timer->timeout <= t->timeout)
		{
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return;
		}
	}
}

//取消定时器
int timer_cancel(struct TIMER*timer)
{
	int e;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();//在设置过程中禁止改变定时器状态
	if(timer->flags == TIMER_FLAGS_USING)//取消正在使用的
	{
		if(timer == timerctl.t0)//如果取消的是第一个
		{
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		}else
		{
			t = timerctl.t0;
			for(;;)
			{
				if(t->next == timer)
				{
					break;
				}
				
				t = t->next;
			}
			
			t->next = timer->next;
		}
		
		timer->flags = TIMER_FLAGS_ALLOC;
		io_store_eflags(e);
		return 1;
	}
	io_store_eflags(e);
	return 0;
}
void timer_cancelall(struct FIFO32*fifo)
{
	int e, i;
	struct TIMER*t;
	e = io_load_eflags();
	io_cli();
	for(i = 0; i < MAX_TIMER; i++)
	{
		t = &timerctl.timer0[i];
		if(t->flags != 0 && t->flags2 != 0 && t->fifo == fifo)
		{
			timer_cancel(t);
			timer_free(t);
		}
	}
}

