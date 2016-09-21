#include "apilib.h"
#include <stdio.h>
#include <stdarg.h>
int printf(char *format, ...)
{
	va_list ap;
	char s[1000];
	int i;
	
	va_start(ap, format);
	i = vsprintf(s, format, ap);
	api_putstr(s);
	va_end();
	return i;
}