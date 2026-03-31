#' Chromatic aberration effect
#'
#' @param nr A `nativeRaster` object.
#' @param r,g,b A numeric vector of length 6;
#'  X and Y offsets for the red, green and blue channels
#'  (in uv coordinates).
#' @returns A `nativeRaster` object.
#' @export
rgb_split <- function(nr, r = c(.01, 0), g = c(0, .01), b = c(0, 0)) {
  if (
    !all(is.finite(c(r, g, b))) ||
      !all(length(r) == 2, length(g) == 2, length(b) == 2)
  ) {
    cli::cli_abort("uniforms must be finite numerics length 2.")
  }
  uniforms <- list(
    uRed = as.double(r),
    uGreen = as.double(g),
    uBlue = as.double(b)
  )
  shdr_rgb_split(nr, uniforms)
}
