#' Shockwave effect
#'
#' @param nr A `nativeRaster` object.
#' @param center A numeric vector of length 2;
#'  the center coordinates where the shockwave starts.
#' @param speed A numeric scalar;
#'  the speed about the shockwave ripples out (in pixels per `time`).
#' @param amp A numeric scalar;
#'  the amplitude of the shockwave.
#' @param wave_length A numeric scalar;
#'  the wave length of the shockwave.
#' @param brigthness A numeric scalar;
#'  the brigthness of the shockwave.
#' @param radius A numeric scalar;
#'  The maximum radius of the shockwave. Set to `-1` for infinite radius.
#' @param time A numeric scalar; ellapsed time.
#' @returns A `nativeRaster` object.
#' @export
shockwave <- function(
  nr,
  center = dim(nr) / 2,
  speed = log(min(dim(nr))),
  amp = 50,
  wave_length = 160,
  brigthness = 1,
  radius = -1,
  time = 1
) {
  if (
    !all(is.finite(c(
      center[1:2],
      speed,
      amp,
      wave_length,
      brigthness,
      radius,
      time
    )))
  ) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  uniforms <-
    list(
      uCenter = as.double(center[1:2]),
      uTime = as.double(time[1]),
      uSpeed = as.double(speed[1]),
      uWave = as.double(c(amp[1], wave_length[1], brigthness[1], radius[1]))
    )
  shdr_shockwave(nr, uniforms)
}
