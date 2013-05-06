#include "..\apilib.h"
#define N 1000000
#define SHIFT 5
#define BITSPERWORD 32
#define MAKS 0x1f
int bitmap[1 + N/BITSPERWORD];
void set(int i){bitmap[i >> SHIFT] |= (1 << (i & MASK));}
void clr(int i){bitmap[i >> SHIFT] &= ~(1 << (i & MASK));}
int test(int i){return bitmap[i >> SHIFT] & (1 <<(i & MASK);}