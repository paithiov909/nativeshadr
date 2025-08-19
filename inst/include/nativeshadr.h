#pragma once
#include "hlsl++.h"
#include <Rcpp.h>

using namespace hlslpp;

inline int4 icol_to_int4(uint32_t icol) {
  return int4(icol & 0xFF, (icol >> 8) & 0xFF, (icol >> 16) & 0xFF,
              (icol >> 24) & 0xFF);
}

inline uint32_t int4_to_icol(int4 col) {
  return col.x | (col.y << 8) | (col.z << 16) | (col.w << 24);
}

inline int4 texture_eval(const Rcpp::IntegerMatrix& nr, const int2& wh) {
  const auto dim = std::make_tuple(nr.nrow(), nr.ncol());  // height, width
  const auto i = clamp(wh.y, 0, std::get<0>(dim));         // height
  const auto j = clamp(wh.x, 0, std::get<1>(dim));         // width
  const int4 ret = icol_to_int4(nr[j + i * std::get<1>(dim)]);
  return ret;
}

inline std::function<Rcpp::IntegerVector(Rcpp::IntegerMatrix, Rcpp::List)>
vectorize_shader(
    std::function<uint32_t(int2, Rcpp::IntegerMatrix, Rcpp::List)> shader) {
  return [shader](Rcpp::IntegerMatrix nr, Rcpp::List uniforms) {
    const auto dim = std::make_tuple(nr.nrow(), nr.ncol());  // height, width
    // Rprintf("dim (height, width): %d, %d\n", std::get<0>(dim), std::get<1>(dim));
    Rcpp::IntegerVector out(std::get<0>(dim) * std::get<1>(dim));
    for (int i = 0; i < std::get<0>(dim); i++) {    // height
      for (int j = 0; j < std::get<1>(dim); j++) {  // width
        out[j + i * std::get<1>(dim)] = shader(int2(j, i), nr, uniforms);
      }
    }
    out.attr("dim") = Rcpp::IntegerVector{std::get<0>(dim), std::get<1>(dim)};
    out.attr("class") = "nativeRaster";
    return out;
  };
}
