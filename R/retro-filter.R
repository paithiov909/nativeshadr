#' Retro look effect
#'
#' @param nr A `nativeRaster` object.
#' @param asp A numeric vector of length 2; factor to change the offset of vignette effect.
#' @param distort A logical scalar; whether to apply barrel distortion.
#' @param time A numeric scalar; ellapsed time.
#' @returns A `nativeRaster` object.
#' @export
retro_filter <- function(nr, asp = rev(dim(nr) / 100), distort = TRUE, time = 0) {
  if (!all(is.finite(c(asp[1:2], time[1])))) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  uniforms <- list(
    uTime = as.double(time[1]),
    uAspect = as.double(asp[1:2]),
    uDistort = if (distort) 1 else 0
  )
  shdr_retrofilter(nr, uniforms)
}
