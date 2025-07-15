#include <stdio.h>

int R0, R1, R2, R3, R4, R5, R6, R7;

void akr1()
{
	if (R0)
		akr2();
	R0 += 1;
}

void akr2()
{
	if (R1)
		goto 0x17a0;
	R0 += 0x7fff;
	R1 = R7;
	akr1();
}