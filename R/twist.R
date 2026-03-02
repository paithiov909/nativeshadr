#' Twist effect
#'
#' @param nr A `nativeRaster` object.
#' @param offset A numeric vector of length 2;
#'  the offset coordinates to change the position of the center of the effect.
#' @param angle A numeric scalar; the angle of the twist.
#' @param radius A numeric scalar; the radius of the twist.
#' @param padding A numeric scalar; padding for the filter area.
#' @returns A `nativeRaster` object.
#' @export
twist <- function(
  nr,
  offset = rev(dim(nr)) / 2,
  angle = pi,
  radius = 100,
  padding = 20
) {
  if (!all(is.finite(c(angle, offset[1:2], radius, padding)))) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  uniforms <-
    list(
      uTwist = as.double(c(radius[1], angle[1])),
      uOffset = as.double(offset[1:2])
    )
  shdr_twist(nr, uniforms, as.double(padding[1]))
}
