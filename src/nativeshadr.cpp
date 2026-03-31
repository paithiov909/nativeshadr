#include "nativeshadr_effects.h"

// [[Rcpp::export]]
Rcpp::IntegerVector test_as_is(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const auto as_is = [](int2 wh, RMatrix<int> nr, const vvd& uniforms) {
    int4 col = texture_eval(nr, wh);
    return int4_to_icol(col);
  };
  const vvd uniforms;
  return vectorize_shader(as_is)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector test_gradient(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const auto gradient = [](int2 wh, RMatrix<int> nr, const vvd& uniforms) {
    float2 uv = float2(wh) / float2(nr.ncol(), nr.nrow());
    float4 col = float4(uv.x, uv.y, .6, 1);
    return int4_to_icol(clamp(col * 255, 0, 255));
  };
  const vvd uniforms;
  return vectorize_shader(gradient)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_sandy(Rcpp::IntegerMatrix nr, float lacunarity,
                               float gain, float intensity, float octaves,
                               float time) {
  time += M_1_PI;
  const float1 _intensity = clamp(float1(intensity), 0, 1);
  const float1 _octaves = clamp(float1(octaves), 1, 8);
  const auto shade = [&lacunarity, &gain, &_intensity, &_octaves, &time](
                         int2 wh, RMatrix<int> nr, const vvd& uniforms) {
    float2 uv = float2(wh) / float2(nr.ncol(), nr.nrow());
    float4 col = texture(nr, uv);
    float3 rep = float3(uv.x, uv.y, time);
    float1 t = lerp(1 + Perlin::turb(_octaves, col.xyz, rep, lacunarity, gain),
                    0.0, _intensity);
    col.rgb *= t.xxx;
    return int4_to_icol(clamp(col * 255, 0, 255));
  };
  const vvd uniforms;
  return vectorize_shader(shade)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_asciifilter(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const vvd uniforms = {uni["uScale"], uni["uIntensity"]};
  return vectorize_shader(Effects::ascii_filter)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_deform(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const vvd uniforms = {uni["uAmplitude"], uni["uFreq"]};
  return vectorize_shader(Effects::deform)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_godray(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const double w = static_cast<double>(nr.ncol());
  const double h = static_cast<double>(nr.nrow());
  const std::vector<double> octaves = uni["uOctaves"];
  const float1 _octaves = clamp(float1(octaves[0]), 1, 8);
  const vvd uniforms = {{w, h},       uni["uParallel"], uni["uLight"], {h / w},
                        uni["uTime"], uni["uRay"],      {_octaves}};
  return vectorize_shader(Effects::godray)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_retrofilter(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const vvd uniforms = {uni["uAspect"], uni["uDistort"], uni["uTime"]};
  return vectorize_shader(Effects::retro_filter)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_rgb_split(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const vvd uniforms = {uni["uRed"], uni["uGreen"], uni["uBlue"]};
  return vectorize_shader(Effects::rgb_split)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_ripple(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const vvd uniforms = {uni["uCenter"], uni["uFreq"], uni["uAmp"]};
  return vectorize_shader(Effects::ripple)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_shockwave(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const double w = static_cast<double>(nr.ncol());
  const double h = static_cast<double>(nr.nrow());
  const vvd uniforms = {uni["uCenter"], uni["uTime"], uni["uSpeed"],
                        uni["uWave"],   {w, h, 0, 0}, {0, 0, .996, .996}};
  return vectorize_shader(Effects::shockwave)(nr, uniforms);
}

// [[Rcpp::export]]
Rcpp::IntegerVector shdr_twist(Rcpp::IntegerMatrix nr, Rcpp::List uni,
                               double padding) {
  const double w = static_cast<double>(nr.ncol());
  const double h = static_cast<double>(nr.nrow());
  const vvd uniforms = {
      uni["uTwist"], uni["uOffset"], {w, h, padding, padding}};
  return vectorize_shader(Effects::twist)(nr, uniforms);
}
