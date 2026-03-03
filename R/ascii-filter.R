#' ASCII filter
#'
#' @param nr A `nativeRaster` object.
#' @param sc_factor A numeric scalar; the scale factor.
#' @param intensity A numeric scalar; the intensity of filter edge.
#'  Set to `0` to disable the filter.
#' @returns A `nativeRaster` object.
#' @export
ascii_filter <- function(nr, sc_factor = 128, intensity = 0.8) {
  if (!all(is.finite(c(sc_factor[1], intensity[1])))) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  if (sc_factor[1] < 1 || sc_factor[1] > 512) {
    sc_factor[1] <- clamp(sc_factor[1], 1, 512)
    cli::cli_warn("sc_factor is clamped between 1 and 512.")
  }
  uniforms <- list(
    uScale = as.double(sc_factor[1]),
    uIntensity = 1 - clamp(intensity[1], 0, 1)
  )
  shdr_asciifilter(nr, uniforms)
}
