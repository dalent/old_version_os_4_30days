#include "system.h"
struct TIMER*task_timer;
struct TASKCTL * taskctl;
void task_idle(void);
struct TASK* task_init(struct MEMMAN*memman)
{
	int i;
	struct TASK *task, *idle;
	taskctl = (struct TASKCTL*)memman_alloc_4k(memman,sizeof(struct TASKCTL));
	for(i = 0; i < MAX_TASKS; i++)
	{
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel   =  (TASK_GDT0 + i) * 8;
		taskctl->tasks0[i].tss.ldtr = (TASK_GDT0 + MAX_TASKS + i) * 8;
		gdt_set_gate(TASK_GDT0 + i, (int)&taskctl->tasks0[i].tss, 103, 0x89, 0x00);//设置任务对应的gdt
		gdt_set_gate(TASK_GDT0 + i + MAX_TASKS, (int)taskctl->tasks0[i].ldt, 15, 0x82, 0x00);
	}	
	
	for(i = 0; i < MAX_TASKLEVELS; i++)
	{
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}
	
	task = task_alloc();//当前任务，linux中的任务0
	task->flags = 2;//活动中标志
	task->priority = 2;
	task->level = 0;
	task_add(task);
	task_switchsub();
	load_tr(task->sel);
	
	task_timer = timer_alloc();
	timer_settime(task_timer, 2);
	
	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int)&task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	task_run(idle, MAX_TASKLEVELS - 1, 1);
	
	return task;
}

struct TASK* task_alloc(void)
{
	int i;
	struct TASK*task;
	for(i = 0; i< MAX_TASKS; i++)
	{
		if(taskctl->tasks0[i].flags == 0)
		{
			task = &taskctl->tasks0[i];
			task->flags = 1;//正在使用标志
			task->tss.eflags = 0x202;
			task->tss.eax = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es  = 0;
			task->tss.ds  = 0;
			task->tss.fs  = 0;
			task->tss.gs  = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0;//全部正在使用。
}

/*返回当前的任务*/
struct TASK* task_now(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

//增加一个任务
void task_add(struct TASK*task)
{
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2;
	return;
}

//删除一个任务
void task_remove(struct TASK*task)
{
	int i;
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	for(i = 0; i < tl->running; i++)
	{
		if(tl->tasks[i] == task)
			break;
	}
	tl->running--;
	if(i < tl->now)
		tl->now--;
	if(tl->now >= tl->running)
		tl->now = 0;
	task->flags = 1;
	for(;i < tl->running; i++)
	{
		tl->tasks[i] = tl->tasks[i + 1];
	}
}

//切换任务等级
void task_switchsub(void)
{
	int i;
	//寻找最上层的level
	for(i = 0; i < MAX_TASKLEVELS; i++)
	{
		if(taskctl->level[i].running > 0)
			break;// 找到啦
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}


void task_run(struct TASK*task, int level, int priority)
{
	if(level < 0)
		level = task->level;
	if(priority > 0)
	{
		task->priority = priority;
	}
	if(task->flags == 2 && task->level != level)
	{
		task_remove(task);
	}
	if(task->flags != 2)
	{
		task->level = level;
		task_add(task);
	}
	
	taskctl->lv_change = 1;//下次任务切换时检查一下
	return;
}

void task_switch(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK*new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if(tl->now == tl->running)
		tl->now = 0;
	if(taskctl->lv_change != 0)
	{
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
}


void task_sleep(struct TASK*task)
{
	struct TASK *now_task;
	if(task->flags == 2)//得是正在运行的程序嘛
	{
		now_task = task_now();//先保存住当前的句柄
		task_remove(task);//删除沉睡的任务
		if(task == now_task)//如果当前的和删除的是一个，我们得任务切换啦
		{
			task_switchsub();
			now_task = task_now();
			if(task == now_task)
				for(;;);
			farjmp(0, now_task->sel);
		}
	}
	return;
}
//空闲任务
void task_idle(void)
{
	for(;;)
		io_hlt();
}