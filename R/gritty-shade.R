#' Multiply turbulence to the image
#'
#' @param nr A `nativeRaster` object.
#' @param lacunarity A numeric scalar; the lacunarity of the effect.
#' @param gain A numeric scalar; the gain of the effect.
#' @param intensity A numeric scalar; the intensity of the effect.
#' @returns A `nativeRaster` object.
#' @export
gritty_shade <- function(nr, lacunarity = 2, gain = 1.2, intensity = 0.4) {
  if (!all(is.finite(c(lacunarity, gain, intensity)))) {
    cli::cli_abort("uniforms must be finite numerics.")
  }
  shdr_sandy(nr, as.double(lacunarity[1]), as.double(gain[1]), as.double(intensity[1]))
}
