#' Deformation
#'
#' Deforms the image by sampling it at
#' `uv + float2(sin(uv.y * freq.x) * amp.x, cos(uv.x * freq.y) * amp.y)`.
#'
#' @param nr A `nativeRaster` object.
#' @param freq A numeric vector of length 2;
#'  the frequency of the deformation.
#' @param amp A numeric vector of length 2;
#'  the amplitude of the deformation.
#' @returns A `nativeRaster` object.
#' @export
deform <- function(nr, freq = c(10, 10), amp = c(0.1, 0.1)) {
  if (!all(is.finite(freq)) || length(freq) != 2) {
    cli::cli_abort("`freq` must be finite numerics length 2.")
  }
  if (!all(is.finite(amp)) || length(amp) != 2) {
    cli::cli_abort("`amount` must be finite numerics length 2.")
  }
  uniforms <- list(uAmplitude = as.double(amp), uFreq = as.double(freq))
  shdr_deform(nr, uniforms)
}
