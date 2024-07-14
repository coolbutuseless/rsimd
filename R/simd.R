


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Multiply and add
#' 
#' @param a,b,c numeric vectors. Must all be of equal length
#' @return a * b + c
#' @examples
#' multiply_add_r(2, 3, 4)
#' multiply_add_c(2, 3, 4)
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
multiply_add_r <- function(a, b, c) {
  stopifnot(length(a) == length(b) && length(a) == length(c))
  a * b + c
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname multiply_add_r
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
multiply_add_c <- function(a, b, c) {
  .Call(multiply_add_c_, a, b, c)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname multiply_add_r
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
multiply_add_unrolled <- function(a, b, c) {
  .Call(multiply_add_unrolled_, a, b, c)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname multiply_add_r
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
multiply_add_simde <- function(a, b, c) {
  .Call(multiply_add_simde_, a, b, c)
}



if (FALSE) {
  
  N <- 1000
  a <- runif(N)
  b <- runif(N)
  c <- runif(N)
  
  # multiply_add_simd_custom(a, b, c) -> zz
  
  bench::mark(
    multiply_add_r(a, b, c),
    multiply_add_c(a, b, c),
    multiply_add_unrolled(a, b, c),
    multiply_add_simde(a, b, c)
  )
  
}