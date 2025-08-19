test_that("test_as_is does not change the image", {
  vdiffr::expect_doppelganger("as-is", {
    nr <-
      fastpng::read_png(
        system.file("images/river.png", package = "nativeshadr"),
        type = "nativeraster"
      )
    plot(test_as_is(nr, list()))
    invisible(grDevices::recordPlot())
  })
})

test_that("test_gradient looks well", {
  vdiffr::expect_doppelganger("gradient", {
    plot(test_gradient(nr_new(320, 240), list()))
    invisible(grDevices::recordPlot())
  })
})
