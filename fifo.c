#include"system.h"
#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo,int size, int *buf, struct TASK* task)//初始化
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->p = 0;
	fifo->q = 0;
	fifo->free = size;
	fifo->flags = 0;
	fifo->task = task;
	return;
}

int fifo32_put(struct FIFO32*fifo, int data)//给fifo发送数据并存储在FIFO中
{
	if(fifo->free == 0)
	{
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p++] = data;
	fifo->p %= fifo->size;
	fifo->free--;
	if(fifo->task != 0)
	{
		if(fifo->task->flags != 2)
		{
			task_run(fifo->task, -1, 0);
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32*fifo)//从FIFO中取得数据	
{
	int data;
	if(fifo->free == fifo->size)
	{
		return -1;
	}
	data = fifo->buf[fifo->q++];
	fifo->q %= fifo->size;
	fifo->free++;
	return data;
	
}

int fifo32_status(struct FIFO32*fifo)//报告已经存储了多少数据
{
	return fifo->size - fifo->free;
}