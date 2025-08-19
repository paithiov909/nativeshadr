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

inline int4 texture_eval(const Rcpp::IntegerMatrix& nr,
                         const std::vector<int>& hw) {
  if (hw.size() != 2) {
    throw std::runtime_error("hw must have length 2");
  }
  const auto dim = std::make_tuple(nr.nrow(), nr.ncol());
  const auto h = std::clamp(hw[0], 0, std::get<0>(dim));
  const auto w = std::clamp(hw[1], 0, std::get<1>(dim));
  const int4 ret = icol_to_int4(nr[h + w * std::get<0>(dim)]);
  return ret;
}

inline std::function<Rcpp::IntegerVector(Rcpp::IntegerMatrix, Rcpp::List)>
vectorize_shader(
    std::function<uint32_t(Rcpp::IntegerMatrix, std::vector<int>, Rcpp::List)>
        shader) {
  return [shader](Rcpp::IntegerMatrix nr, Rcpp::List uniforms) {
    const auto dim = std::make_tuple(nr.nrow(), nr.ncol()); // height, width
    Rprintf("dim: %d, %d\n", std::get<0>(dim), std::get<1>(dim));
    Rcpp::IntegerVector ret(std::get<0>(dim) * std::get<1>(dim));
    for (int i = 0; i < std::get<0>(dim); i++) { // height
      for (int j = 0; j < std::get<1>(dim); j++) { // width
        ret[i + j * std::get<0>(dim)] = shader(nr, std::vector<int>{i, j}, uniforms);
      }
    }
    ret.attr("dim") = Rcpp::IntegerVector{std::get<0>(dim), std::get<1>(dim)};
    ret.attr("class") = "nativeRaster";
    return ret;
  };
}
