#include "apilib.h"
void main()
{
	static char chinese[] = {0xd6,0xd0,0xb9,0xfa,0xd3,0xef,0x68,0x7a,0x6b,0x31,0x36};
	int langmode = api_getlang();
	if(langmode == 0)
		api_putstr("English ASCII mode \n");
	else if (langmode == 1)
	{
		api_putstr(chinese);
	}
}