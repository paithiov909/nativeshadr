#include "nativeshadr.h"

using namespace hlslpp;

// [[Rcpp::export]]
Rcpp::IntegerVector test_as_is(Rcpp::IntegerMatrix nr, Rcpp::List uniforms) {
  auto as_is = [](Rcpp::IntegerMatrix nr, std::vector<int> xy, Rcpp::List uniforms) {
    int4 col = texture_eval(nr, xy);
    return int4_to_icol(col);
  };
  return vectorize_shader(as_is)(nr, uniforms);
}

uint32_t texture(Rcpp::IntegerMatrix nr, std::vector<int> hw, Rcpp::List uniforms) {
  float2 uv = float2(hw[0], hw[1]) / float2(nr.ncol(), nr.nrow());
  float4 col = float4(uv.x, uv.y, .6, 1);
  return int4_to_icol(clamp(col * 255, 0, 255));
}

// [[Rcpp::export]]
Rcpp::IntegerVector test(Rcpp::IntegerMatrix nr, Rcpp::List uniforms) {
  return vectorize_shader(texture)(nr, uniforms);
}
