#pragma once
#include "hlsl++.h"
#include <Rcpp.h>
#include <RcppParallel.h>

using namespace hlslpp;
using namespace RcppParallel;

// vvd = vector of vector of double
typedef std::vector<std::vector<double>> vvd;

inline int4 icol_to_int4(uint32_t icol) {
  return int4(icol & 0xFF, (icol >> 8) & 0xFF, (icol >> 16) & 0xFF,
              (icol >> 24) & 0xFF);
}

inline uint32_t int4_to_icol(int4 col) {
  return ((uint8_t)col.w << 24) | ((uint8_t)col.z << 16) |
         ((uint8_t)col.y << 8) | (uint8_t)col.x;
}

inline int4 texture_eval(const RcppParallel::RMatrix<int>& nr, const int2& wh) {
  const auto dim = std::make_tuple(nr.nrow(), nr.ncol());  // height, width
  const auto i = clamp(wh.y, 0, std::get<0>(dim));         // height
  const auto j = clamp(wh.x, 0, std::get<1>(dim));         // width
  const int4 ret = icol_to_int4(nr[j + i * std::get<1>(dim)]);
  return ret;
}

inline std::function<Rcpp::IntegerVector(Rcpp::IntegerMatrix, const vvd&)>
vectorize_shader(
    std::function<uint32_t(int2, RcppParallel::RMatrix<int>, const vvd&)>
        shader) {
  struct Shader : RcppParallel::Worker {
    std::function<uint32_t(int2, RcppParallel::RMatrix<int>, const vvd&)>
        shader;
    const RcppParallel::RMatrix<int> nr;
    RcppParallel::RVector<int> ret;
    const vvd& uniforms;

    Shader(std::function<uint32_t(int2, RcppParallel::RMatrix<int>, const vvd&)>
               shader,
           Rcpp::IntegerMatrix nr, Rcpp::IntegerVector ret, const vvd& uniforms)
        : shader(shader), nr(nr), ret(ret), uniforms(uniforms) {}
    void operator()(std::size_t begin, std::size_t end) {
      for (std::size_t i = begin; i < end; i++) {      // height
        for (std::size_t j = 0; j < nr.ncol(); j++) {  // width
          ret[j + i * nr.ncol()] = shader(int2(j, i), nr, uniforms);
        }
      }
    }
  };

  return [shader](Rcpp::IntegerMatrix nr, const vvd& uniforms) {
    const auto dim = std::make_tuple(nr.nrow(), nr.ncol());  // height, width

    Rcpp::IntegerVector out(std::get<0>(dim) * std::get<1>(dim));
    Shader worker(shader, nr, out, uniforms);
    RcppParallel::parallelFor(0, std::get<0>(dim), worker);

    out.attr("dim") = Rcpp::IntegerVector{std::get<0>(dim), std::get<1>(dim)};
    out.attr("class") = "nativeRaster";
    return out;
  };
}
