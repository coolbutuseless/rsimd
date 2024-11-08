
<!-- README.md is generated from README.Rmd. Please edit that file -->

# rsimd

<!-- badges: start -->

![](https://img.shields.io/badge/cool-useless-green.svg)
[![R-CMD-check](https://github.com/coolbutuseless/rsimd/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/coolbutuseless/rsimd/actions/workflows/R-CMD-check.yaml)
<!-- badges: end -->

`{rsimd}` is a demonstration package showing how cross-platform
[SIMD](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data)
instructions can be used in the compiled C code in a package.

This package makes use of
[SIMDe](https://github.com/simd-everywhere/simde) - a header-only C
library which provides a SIMD translation layer such that SIMD
intrinsics from one CPU architecture may be run on a different CPU
archtecture e.g. running AVX512 instructions on ARM Neon.

In this package, SIMD instructions are written in [x86
AVX512](https://en.wikipedia.org/wiki/AVX-512)
[intrinsics](https://en.wikipedia.org/wiki/Intrinsic_function) and
should run natively on modern x86 CPUs.

By using SIMDe library, these x86 AVX512 automatically uses [ARM
Neon](https://en.wikipedia.org/wiki/ARM_architecture_family#Advanced_SIMD_(Neon))
SIMD instructions in order to run fast on Mac AArch64 ARM processors.

### What’s in the box

- `cpuid()` output some very basic CPU ID information about which SIMD
  instructions sets your CPU supports.
- Four implementations of a fused muliply-add
  - `multiply_add_r()` Pure R code which just runs `a * b + c`
  - `multiply_add_c()` Naive C code which does the same operation in C
  - `multiply_add_unrolled()` C code in which the inner for-loop is
    unrolled for performance
  - `multiply_add_simde()` C code including AVX512 SIMD instructions

## Installation

You can install from [GitHub](https://github.com/coolbutuseless/rsimd)
with:

``` r
# install.packages('remotes')
remotes::install_github('coolbutuseless/rsimd')
```

#### OS Notes

The SIMD code should compile and work on both x86 and mac ARM
processors.

Please let me know if this is not the case for your machine.

#### CRAN Notes

SIMDe does some pretty fancy footwork with types and there are currently
a few mismatched types between platform-defined SIMD data types and
those in SIMDe.

This mismatch leads to some warnings when checking with `clang-asan` and
`valgrind`.

This means that CRAN probably won’t currently accept a package using
SIMDe in its current version.

If you were to drop SIMDe and instead write custom SIMD for each
platform and enable with `#ifdef` statements, then the type errors would
not occur, and CRAN should accept the package (if there are no other
issues)

## CPU ID

Print some basic notes about your CPU and the supported SIMD
instructions.

``` r
cpuid()
#> macOS AArch64 (ARM) with NEON
```

## Included case study: Fused multiply-add

The case-study explored in this package is to speed up
`fused multiply add` in R by using SIMD instructions

A common operation is to multiply two vectors and add a constant
e.g. `a * b + c`.

In SIMD, this is sometimes referred to as a ‘fused multiply add’.

The following code benchmarks 4 implementations

General findings on my mac ARM M2 CPU

- Plain R implementation is faster than C
- Unrolled C code is faster than R
- SIMD code is faster than all

``` r
library(rsimd)

N <- 10000
a <- runif(N)
b <- runif(N)
c <- runif(N)

bench::mark(
  multiply_add_r(a, b, c),
  multiply_add_c(a, b, c),
  multiply_add_unrolled(a, b, c),
  multiply_add_simde(a, b, c)
)[, 1:5] |>
  knitr::kable()
```

| expression                     |     min |  median |   itr/sec | mem_alloc |
|:-------------------------------|--------:|--------:|----------:|----------:|
| multiply_add_r(a, b, c)        |  6.76µs | 11.44µs |  82250.34 |    89.1KB |
| multiply_add_c(a, b, c)        | 17.38µs |  20.3µs |  46649.50 |    82.3KB |
| multiply_add_unrolled(a, b, c) |  6.19µs |  9.47µs |  98426.40 |    82.3KB |
| multiply_add_simde(a, b, c)    |  4.67µs |  8.65µs | 107479.19 |    82.3KB |

## R and C code implementations

#### R code from `R/simd.R`

<details>
<summary>
`multiply_add_r()`
</summary>

``` r
multiply_add_r <- function(a, b, c) {
  stopifnot(length(a) == length(b) && length(a) == length(c))
  a * b + c
}
```

</details>

#### The following C code sections are from this package `src/simd.c`

<details>
<summary>
`multiply_add_c()`
</summary>

``` c
SEXP multiply_add_c_(SEXP a_, SEXP b_, SEXP c_) {

  if (length(a_) != length(b_) || length(a_) != length(c_)) {
    error("All arguments must be the same length");
  }
  
  
  double *a = REAL(a_);
  double *b = REAL(b_);
  double *c = REAL(c_);
  
  SEXP res_ = PROTECT(allocVector(REALSXP, length(a_)));
  double *res = REAL(res_);
  
  for (int i = 0; i < length(a_); i++) {
    res[i] = a[i] * b[i] + c[i];
  }
  
  
  UNPROTECT(1);
  return res_;
}
```

</details>
<details>
<summary>
`multiply_add_unrolled()`
</summary>

    SEXP multiply_add_unrolled_(SEXP a_, SEXP b_, SEXP c_) {
      
      if (length(a_) != length(b_) || length(a_) != length(c_)) {
        error("All arguments must be the same length");
      }
      
      
      double *a = REAL(a_);
      double *b = REAL(b_);
      double *c = REAL(c_);
      
      SEXP res_ = PROTECT(allocVector(REALSXP, length(a_)));
      double *res = REAL(res_);

    #define UNROLL 8 
      int i = 0;
      for (; i < length(a_) - (UNROLL - 1); i += UNROLL) {
        *res++ = *a++ * *b++ + *c++;
        *res++ = *a++ * *b++ + *c++;
        *res++ = *a++ * *b++ + *c++;
        *res++ = *a++ * *b++ + *c++;
        
        *res++ = *a++ * *b++ + *c++;
        *res++ = *a++ * *b++ + *c++;
        *res++ = *a++ * *b++ + *c++;
        *res++ = *a++ * *b++ + *c++;
      }
      for (; i< length(a_); i++) {
        res[i] = a[i] * b[i] + c[i];
      }
      
      
      UNPROTECT(1);
      return res_;
    }

</details>
<details>
<summary>
`multiply_add_simde()`
</summary>

    #define SIMDE_ENABLE_NATIVE_ALIASES 1
    #include "simde/x86/avx512.h"

    SEXP multiply_add_simde_(SEXP a_, SEXP b_, SEXP c_) {
      
      if (length(a_) != length(b_) || length(a_) != length(c_)) {
        error("All arguments must be the same length");
      }
      
      
      double *ap = REAL(a_);
      double *bp = REAL(b_);
      double *cp = REAL(c_);
      
      SEXP res_ = PROTECT(allocVector(REALSXP, length(a_)));
      double *resp = REAL(res_);
      
    #define UNROLL 8 
      int i = 0;
      for (; i < length(a_) - (UNROLL - 1); i += UNROLL) {
        __m512d a = _mm512_load_pd(ap + i);
        __m512d b = _mm512_load_pd(bp + i);
        __m512d c = _mm512_load_pd(cp + i);
        __m512d res = _mm512_fmadd_pd(a, b, c);
        _mm512_store_pd(resp + i, res);
      }
      for (; i< length(a_); i++) {
        resp[i] = ap[i] * bp[i] + cp[i];
      }
      
      
      UNPROTECT(1);
      return res_;
    }

</details>

## References

- [Intel’s guide to SIMD
  intrinsics](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)
- [ARM Neon Intrinsics
  Reference](file:///Users/mike/Desktop/video/advsimd-2021Q2.pdf)
- [SIMDe library](https://github.com/simd-everywhere/simde)
