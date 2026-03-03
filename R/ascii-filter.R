#' ASCII filter
#'
#' @param nr A `nativeRaster` object.
#' @param sc_factor A numeric scalar; the scale factor.
#' @returns A `nativeRaster` object.
#' @export
ascii_filter <- function(nr, sc_factor = 128, intensity = 0.2) {
  if (!all(is.finite(c(sc_factor[1], intensity[1])))) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  if (sc_factor[1] < 1 || sc_factor[1] > 512) {
    sc_factor[1] <- clamp(sc_factor[1], 1, 512)
    cli::cli_warn("sc_factor is clamped between 1 and 512.")
  }
  uniforms <- list(uScale = as.double(sc_factor[1]), uIntensity = as.double(intensity[1]))
  shdr_asciifilter(nr, uniforms)
}
