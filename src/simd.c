
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP multiply_add_c_(SEXP a_, SEXP b_, SEXP c_) {

  if (length(a_) != length(b_) || length(a_) != length(c_)) {
    error("All arguments must be the same length");
  }
  
  const double *a = REAL(a_);
  const double *b = REAL(b_);
  const double *c = REAL(c_);
  
  SEXP res_ = PROTECT(allocVector(REALSXP, length(a_)));
  double *res = REAL(res_);
  
  for (int i = 0; i < length(a_); i++) {
    res[i] = a[i] * b[i] + c[i];
  }
  
  
  UNPROTECT(1);
  return res_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP multiply_add_fma_(SEXP a_, SEXP b_, SEXP c_) {
  
  if (length(a_) != length(b_) || length(a_) != length(c_)) {
    error("All arguments must be the same length");
  }
  
  
  double *a = REAL(a_);
  double *b = REAL(b_);
  double *c = REAL(c_);
  
  SEXP res_ = PROTECT(allocVector(REALSXP, length(a_)));
  double *res = REAL(res_);
  
  for (int i = 0; i < length(a_); i++) {
    res[i] = fma(a[i], b[i], c[i]);
  }
  
  
  UNPROTECT(1);
  return res_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
    res[i + 0] = a[i + 0] * b[i + 0] + c[i + 0];
    res[i + 1] = a[i + 1] * b[i + 1] + c[i + 1];
    res[i + 2] = a[i + 2] * b[i + 2] + c[i + 2];
    res[i + 3] = a[i + 3] * b[i + 3] + c[i + 3];
    res[i + 4] = a[i + 4] * b[i + 4] + c[i + 4];
    res[i + 5] = a[i + 5] * b[i + 5] + c[i + 5];
    res[i + 6] = a[i + 6] * b[i + 6] + c[i + 6];
    res[i + 7] = a[i + 7] * b[i + 7] + c[i + 7];
  }
  for (; i< length(a_); i++) {
    res[i] = a[i] * b[i] + c[i];
  }
  
  
  UNPROTECT(1);
  return res_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP multiply_add_unrolled_fma_(SEXP a_, SEXP b_, SEXP c_) {
  
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
    res[i + 0] = fma(a[i + 0], b[i + 0], c[i + 0]);
    res[i + 1] = fma(a[i + 1], b[i + 1], c[i + 1]);
    res[i + 2] = fma(a[i + 2], b[i + 2], c[i + 2]);
    res[i + 3] = fma(a[i + 3], b[i + 3], c[i + 3]);
    res[i + 4] = fma(a[i + 4], b[i + 4], c[i + 4]);
    res[i + 5] = fma(a[i + 5], b[i + 5], c[i + 5]);
    res[i + 6] = fma(a[i + 6], b[i + 6], c[i + 6]);
    res[i + 7] = fma(a[i + 7], b[i + 7], c[i + 7]);
  }
  for (; i< length(a_); i++) {
    res[i] = fma(a[i], b[i], c[i]);
  }
  
  
  UNPROTECT(1);
  return res_;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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



