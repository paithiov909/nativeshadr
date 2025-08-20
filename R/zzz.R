.onUnload <- function(libpath) {
  library.dynam.unload("nativeshadr", libpath)
}
