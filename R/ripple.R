#' Ripple effect
#'
#' Adds a ripple effect to the image by sampling it at
#' `uv + sin(length(uv - center) * freq) * amp`.
#'
#' @param nr A `nativeRaster` object.
#' @param center A numeric vector of length 2.
#'  The distortion of the effect increases the farther they are from this point.
#' @param freq A numeric scalar; the frequency of the effect.
#' @param amp A numeric scalar; the amplitude of the effect.
#' @returns A `nativeRaster` object.
#' @export
ripple <- function(nr, center = rev(dim(nr)) / 2, freq = 50, amp = 0.01) {
  if (!all(is.finite(c(center[1:2], freq, amp)))) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  uniforms <- list(
    uCenter = as.double(center),
    uFreq = as.double(freq[1]),
    uAmp = as.double(amp[1])
  )
  shdr_ripple(nr, uniforms)
}
