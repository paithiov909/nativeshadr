test_that("test_as_is does not change the image", {
  vdiffr::expect_doppelganger("as-is", {
    nr <-
      fastpng::read_png(
        system.file("images/river.png", package = "nativeshadr"),
        type = "nativeraster"
      )
    grid::grid.newpage()
    grid::grid.raster(test_as_is(nr, list()), interpolate = FALSE)
    invisible(grDevices::recordPlot())
  })
})

test_that("test_gradient looks well", {
  vdiffr::expect_doppelganger("gradient", {
    grid::grid.newpage()
    grid::grid.raster(
      test_gradient(fill_with("white", 320, 240), list()),
      interpolate = FALSE
    )
    invisible(grDevices::recordPlot())
  })
})
