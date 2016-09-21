void * malloc(int size)
{
	char *p = api_malloc(size + 16);
	if(p != 0)
	{
		*((int*)p) = size;
		p += 16;
	}
	return p;
}

void free(void *p)
{
	char *q = p;
	int size;
	if(*q != 0)
	{
		q -= 16;
		size = *((int *)q);
		api_free(q, size + 16);
	}
}