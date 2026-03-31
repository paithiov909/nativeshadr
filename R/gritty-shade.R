#' Multiply turbulence to the image
#'
#' @param nr A `nativeRaster` object.
#' @param lacunarity A numeric scalar; the lacunarity of the effect.
#' @param gain A numeric scalar; the gain of the effect.
#' @param intensity A numeric scalar; the intensity of the effect.
#' @param octaves A numeric scalar; the octaves of turbulence.
#' @param time A numeric scalar; ellapsed time.
#' @returns A `nativeRaster` object.
#' @export
gritty_shade <- function(
  nr,
  lacunarity = 2,
  gain = 1.2,
  intensity = 0.4,
  octaves = 5,
  time = 0
) {
  if (!all(is.finite(c(lacunarity, gain, intensity, octaves, time)))) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  shdr_sandy(
    nr,
    as.double(lacunarity[1]),
    as.double(gain[1]),
    as.double(intensity[1]),
    as.double(octaves[1]),
    as.double(time[1])
  )
}
