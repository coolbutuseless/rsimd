
// #define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern SEXP cpuid_(void);

extern SEXP multiply_add_c_          (SEXP a_, SEXP b_, SEXP c_);
extern SEXP multiply_add_unrolled_   (SEXP a_, SEXP b_, SEXP c_);
extern SEXP multiply_add_simde_      (SEXP a_, SEXP b_, SEXP c_);

static const R_CallMethodDef CEntries[] = {
  
  {"cpuid_", (DL_FUNC) &cpuid_, 0},
  
  {"multiply_add_c_"          , (DL_FUNC) &multiply_add_c_          , 3},
  {"multiply_add_unrolled_"   , (DL_FUNC) &multiply_add_unrolled_   , 3},
  {"multiply_add_simde_"      , (DL_FUNC) &multiply_add_simde_      , 3},
  {NULL , NULL, 0}
};


void R_init_rsimd(DllInfo *info) {
  R_registerRoutines(
    info,      // DllInfo
    NULL,      // .C
    CEntries,  // .Call
    NULL,      // Fortran
    NULL       // External
  );
  R_useDynamicSymbols(info, FALSE);
}



