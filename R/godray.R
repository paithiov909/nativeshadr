#' Godray effect
#'
#' @param nr A `nativeRaster` object.
#' @param parallel A logical scalar; whether to use parallel lighting.
#' @param angle A numeric scalar; the angle of the light.
#'  If `parallel` is `FALSE`, this is ignored.
#' @param center A numeric vector of length 2;
#'  the offset coordinates to roughly change the position of the light.
#'  If `parallel` is `TRUE`, this is ignored.
#' @param gain A numeric scalar; the gain of the effect.
#' @param lacunarity A numeric scalar; the lacunarity of the effect.
#' @param alpha A numeric scalar; the alpha of the effect.
#' @param octaves A numeric scalar; the octaves of turbulence.
#' @param time A numeric scalar; ellapsed time.
#' @returns A `nativeRaster` object.
#' @export
godray <- function(
  nr,
  parallel = FALSE,
  angle = pi / 6,
  center = c(0, 0),
  gain = 0.5,
  lacunarity = 2.5,
  alpha = 1,
  octaves = 5,
  time = 0
) {
  if (
    !all(is.finite(c(
      angle,
      center[1:2],
      gain,
      lacunarity,
      alpha,
      octaves,
      time
    )))
  ) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  uniforms <- list(
    uLight = if (parallel) {
      c(cos(angle[1]), sin(angle[1]))
    } else {
      as.double(center[1:2])
    },
    uRay = as.double(c(gain[1], lacunarity[1], alpha[1])),
    uOctaves = as.double(octaves[1]),
    uTime = as.double(time[1]),
    uParallel = if (parallel) 1 else 0
  )
  shdr_godray(nr, uniforms)
}
