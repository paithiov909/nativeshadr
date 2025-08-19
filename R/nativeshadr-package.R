## usethis namespace: start
#' @import nara
#' @importFrom Rcpp sourceCpp
#' @importFrom RcppParallel RcppParallelLibs
#' @useDynLib nativeshadr, .registration = TRUE
## usethis namespace: end
#' @keywords internal
"_PACKAGE"

#' @inherit nara::nr_new title description details return
#' @inheritParams nara::nr_new
#' @export
#' @keywords internal
nr_new <- function(width, height, fill = "white") {
  nara::nr_new(width, height, fill)
}
