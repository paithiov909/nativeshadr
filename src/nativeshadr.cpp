#include "nativeshadr.h"

// [[Rcpp::export]]
Rcpp::IntegerVector test_as_is(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  auto as_is = [](int2 wh, RMatrix<int> nr, const vvd& uniforms) {
    int4 col = texture_eval(nr, wh);
    return int4_to_icol(col);
  };
  const vvd uniforms;
  return vectorize_shader(as_is)(nr, uniforms);
}

uint32_t gradient(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  float2 uv = float2(wh) / float2(nr.ncol(), nr.nrow());
  float4 col = float4(uv.x, uv.y, .6, 1);
  return int4_to_icol(clamp(col * 255, 0, 255));
}

// [[Rcpp::export]]
Rcpp::IntegerVector test_gradient(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const vvd uniforms;
  return vectorize_shader(gradient)(nr, uniforms);
}
