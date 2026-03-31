## usethis namespace: start
#' @importFrom Rcpp sourceCpp
#' @importFrom RcppParallel RcppParallelLibs
#' @useDynLib nativeshadr, .registration = TRUE
## usethis namespace: end
#' @keywords internal
"_PACKAGE"

#' Create a native raster filled with a color
#'
#' @param color Color name or hex code.
#' @param width,height A positive integer scalar.
#' @returns A `nativeRaster` object.
#' @keywords internal
fill_with <- function(color, width, height) {
  packed_int <- colorfast::col_to_int(color[1])
  out <- matrix(packed_int, nrow = height, ncol = width)
  class(out) <- "nativeRaster"
  out
}

clamp <- function(x, min, max) {
  pmin(pmax(x, min), max)
}
