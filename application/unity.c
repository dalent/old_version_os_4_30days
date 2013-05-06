
void itoa10(int num, char* des)
{
	char* tmp = des;
	char c;
	int val = num;
	do{
		*tmp++ = val % 10 + '0';
		val = val / 10;
	}while(val > 0);
	*tmp = 0;
	tmp--;
	while(tmp > des)
	{
		c = *des;
		*des++ = *tmp;
		*tmp-- = c;
	}
		
}

void sprintf(char * des, const char* format, int num)
{
	char tmp[10];
	char * tm = &tmp[0];
	while(*format != '%') *des++ = *format++;
	if(*format != 0)
	{
		format += 2;
		itoa10(num, tmp);
		while(*des++ = *tm++);
		des--;
	}
	while(*format != '%' && *format != 0)*des++ = *format++;
	*des = 0;
	
}
void memcpy(char * dest, const char * src)
{
	while(*dest++ = *src++);
}

void *strcpy(void *dest, const void *src, unsigned int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

void *memset(void *dest, char val, unsigned int count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned int  strlen(const char *str)
{
    unsigned int retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}
int strcmp(const char * first, const char * second)
{
	while(*first != 0 && * second != 0 && *first++ == *second++);
	if(*first == 0 && *second == 0)
		return 0;
	return 1;
}

int strncmp(const char * first, const char * second, unsigned int size)
{
	while(size--)
	{
		if(*first++ != *second++)
			return -1;
	}
	return 0;
}