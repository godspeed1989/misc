#include <stdio.h>
#include <string.h>
typedef unsigned int u32;

/**
 * Ref arch/x86/include/asm/cpufeature.h
 */
/* Intel-defined CPU features, CPUID level 0x00000001 (edx) */
#define X86_FEATURE_FPU     ( 0) /* Onboard FPU */
#define X86_FEATURE_TSC     ( 4) /* Time Stamp Counter */
#define X86_FEATURE_MSR     ( 5) /* Model-Specific Registers */
#define X86_FEATURE_PAE     ( 6) /* Physical Address Extensions */
#define X86_FEATURE_XMM     (25) /* "sse" */
#define X86_FEATURE_XMM2    (26) /* "sse2" */
#define X86_FEATURE_HT      (28) /* Hyper-Threading */
#define X86_FEATURE_IA64    (30) /* IA-64 processor */

/* Intel-defined CPU features, CPUID level 0x00000001 (ecx) */
#define X86_FEATURE_XMM3    ( 0) /* "pni" SSE-3 */
#define X86_FEATURE_VMX     ( 5) /* Hardware virtualization */
#define X86_FEATURE_SSSE3   ( 9) /* Supplemental SSE-3 */
#define X86_FEATURE_XMM4_1  (19) /* "sse4_1" SSE-4.1 */
#define X86_FEATURE_XMM4_2  (20) /* "sse4_2" SSE-4.2 */
#define X86_FEATURE_AES     (25) /* AES instructions */

/* Intel-defined CPU features, CPUID level 0x00000007 (ebx) */
#define X86_FEATURE_SHA     (29) /* SHA instructions http://software.intel.com/en-us/articles/intel-sha-extensions */

void cpuid(u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	asm volatile(
					"mov %%ebx, %%edi;" /* 32bit PIC: don't clobber ebx */
					"cpuid;"
					"mov %%ebx, %%esi;"
					"mov %%edi, %%ebx;"
					:"+a" (*eax), "=S" (*ebx), "=c" (*ecx), "=d" (*edx)
					:"c" (*ecx)
					:"edi"
				);
}

#define test_bit(name,dat,bit)  \
	do{								\
		printf("%s\t\t", name);	\
		if(dat & (0x1 << bit))		\
			printf("YES\n");		\
		else						\
			printf("NO\n");			\
	}while(0);						\

void cpuid_parse()
{
	u32 eax, ebx, ecx, edx;

	eax = ebx = 0;
	cpuid(&eax, &ebx, &ecx, &edx);
	char str[12+1];
	memcpy(str+0, &ebx, 4);
	memcpy(str+4, &edx, 4);
	memcpy(str+8, &ecx, 4);
	str[12] = '\0';
	printf("'%s'\n", str);
	if(strncmp("GenuineIntel", str, 12))
		printf("Just for Intel CPU\n");

	eax = 1;
	cpuid(&eax, &ebx, &ecx, &edx);
	// CPUID level 0x00000001 (edx)
	test_bit("FPU", edx, X86_FEATURE_FPU);
	test_bit("TSC", edx, X86_FEATURE_TSC);
	test_bit("MSR", edx, X86_FEATURE_MSR);
	test_bit("PAE", edx, X86_FEATURE_PAE);
	test_bit("SSE", edx, X86_FEATURE_XMM);
	test_bit("SSE2", edx, X86_FEATURE_XMM2);
	test_bit("HT", edx, X86_FEATURE_HT);
	test_bit("IA64", edx, X86_FEATURE_IA64);
	// CPUID level 0x00000001 (ecx)
	test_bit("SSE3", ecx, X86_FEATURE_XMM3);
	test_bit("VMX", ecx, X86_FEATURE_VMX);
	test_bit("SSSE3", ecx, X86_FEATURE_SSSE3);
	test_bit("SSE4.1", ecx, X86_FEATURE_XMM4_1);
	test_bit("SSE4.2", ecx, X86_FEATURE_XMM4_2);
	test_bit("AES", ecx, X86_FEATURE_AES);
	
	eax = 7;
	ecx = 0;
	// CPUID level 0x00000007 (ebx)
	cpuid(&eax, &ebx, &ecx, &edx);
	test_bit("SHA", ebx, X86_FEATURE_SHA);
}

int main()
{
	cpuid_parse();
	return 0;
}

