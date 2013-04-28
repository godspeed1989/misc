#include <stdio.h>

#define X86_FEATHER_LM  (1<<29)

void cpuid(int op, unsigned int *eax, unsigned int *ebx,
				   unsigned int *ecx, unsigned int *edx)
{
	__asm__("cpuid" : "=a" (*eax), "=b" (*ebx),
					  "=c" (*ecx), "=d" (*edx)
					: "0" (op));
}

int main()
{
	unsigned int eax, ebx, ecx, edx;
	cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
	if(eax < 0x80000001)
		goto no_longmode;
	
	cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
	if(!(X86_FEATHER_LM & edx))
		goto no_longmode;
	
	printf("Support long mode.\n");	
	return 0;
no_longmode:
	printf("Not support long mode.\n");
	return 1;
}

