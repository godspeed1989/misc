////////////////////////////////////////////////////////////
// testccpuidc.c : [C] 测试ccpuid.h, 显示CPUID信息.
// Author: zyl910
// Blog: http://www.cnblogs.com/zyl910
// URL: http://www.cnblogs.com/zyl910/archive/2012/10/13/ccpuid_v103.html
// Version: V1.03
// Updata: 2012-10-12
//
// Update
// ~~~~~~
//
// [2012-10-12] V1.03
// 同步版本号。其他未改。
//
// [2012-09-29] V1.02
// 分成纯C版和C++版。纯C版使用ccpuid.h。
//
// [2012-08-22] V1.01
// 见testccpuid.cpp.
//
// [2012-07-11] V1.00
// 见testccpuid.cpp.
//
////////////////////////////////////////////////////////////
// gcc -o testccpuidc testccpuidc.c


#include <stdio.h>

#include "ccpuid.h"


// 获取程序位数（被编译为多少位的代码）
int GetProgramBits()
{
	return sizeof(int*) * 8;
}

int main(int argc, char* argv[])
{
	char szBuf[64];
	int bhwmmx;	// 硬件支持MMX.
	int bmmx;	// 操作系统支持MMX.
	int	nhwsse;	// 硬件支持SSE.
	int	nsse;	// 操作系统支持SSE.
	int	nhwavx;	// 硬件支持AVX.
	int	navx;	// 操作系统支持AVX.

	printf("testccpuidc v1.03 (%dbit)\n\n", GetProgramBits());

	// base
	cpu_getvendor(szBuf);
	printf("CPU Vendor:\t%s\n", szBuf);
	cpu_getbrand(szBuf);
	printf("CPU Brand:\t%s\n", szBuf);
	printf("LFuncStd:\t%.8Xh\n", getcpuidfield(CPUF_LFuncStd));
	printf("LFuncExt:\t%.8Xh\n", getcpuidfield(CPUF_LFuncExt));

	// mmx
	bmmx = simd_mmx(&bhwmmx);
	printf("MMX: %d\t// hw: %d\n", bmmx, bhwmmx);

	// sse
	nsse = simd_sse_level(&nhwsse);
	printf("SSE: %d\t// hw: %d\n", nsse, nhwsse);

	// avx
	navx = simd_avx_level(&nhwavx);
	printf("AVX: %d\t// hw: %d\n", navx, nhwavx);

	// misc
	printf("SSE4A:\t%d\n", getcpuidfield(CPUF_SSE4A));
	printf("AES:\t%d\n", getcpuidfield(CPUF_AES));
	printf("PCLMULQDQ:\t%d\n", getcpuidfield(CPUF_PCLMULQDQ));
	printf("F16C:\t%d\n", getcpuidfield(CPUF_F16C));
	printf("FMA:\t%d\n", getcpuidfield(CPUF_FMA));
	printf("FMA4:\t%d\n", getcpuidfield(CPUF_FMA4));
	printf("XOP:\t%d\n", getcpuidfield(CPUF_XOP));

	return 0;
}
