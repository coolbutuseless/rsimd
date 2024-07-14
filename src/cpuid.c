
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CPU ID
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SIMD_SSE     0x1
#define SIMD_SSE2    0x2
#define SIMD_SSE3    0x4
#define SIMD_SSE4_1  0x8
#define SIMD_SSE4_2  0x10
#define SIMD_AVX     0x20
#define SIMD_AVX2    0x40
#define SIMD_AVX512F 0x80

SEXP cpuid_(void) {
#ifdef __ARM_NEON
  Rprintf("macOS AArch64 (ARM) with NEON\n");
#else
  unsigned eax, ebx, ecx, edx;
  asm volatile("cpuid" : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) : "a" (1));
  if (edx>>25&1) { Rprintf("SSE\n"   ); } //flag |= SIMD_SSE;
  if (edx>>26&1) { Rprintf("SSE2\n"  ); }// flag |= SIMD_SSE2;
  if (ecx>>0 &1) { Rprintf("SSE3\n"  ); } // flag |= SIMD_SSE3;
  if (ecx>>19&1) { Rprintf("SSE4.1\n"); } // flag |= SIMD_SSE4_1;
  if (ecx>>20&1) { Rprintf("SSE4.2\n"); } // flag |= SIMD_SSE4_2;
  if (ecx>>28&1) { Rprintf("AVX\n"   ); } // flag |= SIMD_AVX;
  if (ebx>>5 &1) { Rprintf("AVX2\n"  ); } // flag |= SIMD_AVX2;
  if (ebx>>16&1) { Rprintf("AVX512\n"); } // flag |= SIMD_AVX512F;
#endif
  return R_NilValue;
}
