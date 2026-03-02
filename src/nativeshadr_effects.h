// [[Rcpp::depends(RcppParallel)]]
#include "nativeshadr.h"

inline float4 texture(const RMatrix<int>& nr, const float2& uv) {
  const int2 wh{uv.x * nr.ncol(), uv.y * nr.nrow()};
  float4 _texture = texture_eval(nr, wh);
  _texture /= 255;
  return _texture;
}

namespace Perlin {

inline float3 mod289(float3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
inline float4 mod289(float4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
inline float4 permute(float4 x) { return mod289(((x * 34.0) + 1.0) * x); }
inline float4 taylorInvSqrt(float4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}
inline float3 fade(float3 t) {
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}
// Classic Perlin noise, periodic variant
inline float1 pnoise(float3 P, float3 rep) {
  float3 Pi0 = fmod(floor(P), rep);           // Integer part, modulo period
  float3 Pi1 = fmod(Pi0 + float3(1.0), rep);  // Integer part + 1, mod period
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  float3 Pf0 = frac(P);            // Fractional part for interpolation
  float3 Pf1 = Pf0 - float3(1.0);  // Fractional part - 1.0
  float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  float4 iy = float4(Pi0.yy, Pi1.yy);
  float4 iz0 = Pi0.zzzz;
  float4 iz1 = Pi1.zzzz;
  float4 ixy = permute(permute(ix) + iy);
  float4 ixy0 = permute(ixy + iz0);
  float4 ixy1 = permute(ixy + iz1);
  float4 gx0 = ixy0 * (1.0 / 7.0);
  float4 gy0 = frac(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = frac(gx0);
  float4 gz0 = float4(0.5) - abs(gx0) - abs(gy0);
  float4 sz0 = step(gz0, float4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);
  float4 gx1 = ixy1 * (1.0 / 7.0);
  float4 gy1 = frac(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = frac(gx1);
  float4 gz1 = float4(0.5) - abs(gx1) - abs(gy1);
  float4 sz1 = step(gz1, float4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);
  float3 g000 = float3(gx0.x, gy0.x, gz0.x);
  float3 g100 = float3(gx0.y, gy0.y, gz0.y);
  float3 g010 = float3(gx0.z, gy0.z, gz0.z);
  float3 g110 = float3(gx0.w, gy0.w, gz0.w);
  float3 g001 = float3(gx1.x, gy1.x, gz1.x);
  float3 g101 = float3(gx1.y, gy1.y, gz1.y);
  float3 g011 = float3(gx1.z, gy1.z, gz1.z);
  float3 g111 = float3(gx1.w, gy1.w, gz1.w);
  float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010),
                                      dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011),
                                      dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;
  float1 n000 = dot(g000, Pf0);
  float1 n100 = dot(g100, float3(Pf1.x, Pf0.yz));
  float1 n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
  float1 n110 = dot(g110, float3(Pf1.xy, Pf0.z));
  float1 n001 = dot(g001, float3(Pf0.xy, Pf1.z));
  float1 n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
  float1 n011 = dot(g011, float3(Pf0.x, Pf1.yz));
  float1 n111 = dot(g111, Pf1);
  float3 fade_xyz = fade(Pf0);
  float4 n_z = lerp(float4(n000, n100, n010, n110),
                    float4(n001, n101, n011, n111), fade_xyz.zzzz);
  float2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.yy);
  float1 n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}
inline float1 turb(float3 P, float3 rep, float lacunarity, float gain) {
  float sum = 0.0;
  float sc = 1.0;
  float totalgain = 1.0;
  for (float i = 0.0; i < 6.0; i++) {
    sum += totalgain * pnoise(P * sc, rep);
    sc *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}

}  // namespace Perlin

namespace Effects {

inline uint32_t deform(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double> uAmplitude = uniforms[0];  // vec2
  const std::vector<double> uFreq = uniforms[1];       // vec2

  float2 uv = float2(wh) / float2(nr.ncol(), nr.nrow());
  float2 deform = float2(cos(uv.y * uFreq[0]) * uAmplitude[0],
                         sin(uv.x * uFreq[1]) * uAmplitude[1]);
  uv += deform;
  if (uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0) {
    const int4 finalColor = int4(0, 0, 0, 0);
    return int4_to_icol(finalColor);
  }
  float4 deformedColor = texture(nr, uv);

  return int4_to_icol(deformedColor * 255);
}

inline uint32_t godray(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double> uDimensions = uniforms[0];  // vec2
  const std::vector<double> uParallel = uniforms[1];    // float
  const std::vector<double> uLight = uniforms[2];       // vec2
  const std::vector<double> uAspect = uniforms[3];      // float
  const std::vector<double> uTime = uniforms[4];        // float
  const std::vector<double> uRay = uniforms[5];         // vec3
  // const std::vector<double> uInputSize = uniforms[6];   // vec4

  bool parallel = (uParallel[0] > 0.5);
  float2 dim = float2(uDimensions[0], uDimensions[1]);
  float2 light = float2(uLight[0], uLight[1]);
  float1 aspect = uAspect[0];

  float2 coord = float2(wh) / dim;

  float d;

  if (parallel) {
    float _cos = light.x;
    float _sin = light.y;
    d = (_cos * coord.x) + (_sin * coord.y * aspect);
  } else {
    float dx = coord.x - light.x / dim.x;
    float dy = (coord.y - light.y / dim.y) * aspect;
    float dis = sqrt(dx * dx + dy * dy) + 0.00001;
    d = dy / dis;
  }

  float1 time = uTime[0];
  float1 gain = uRay[0];
  float1 lacunarity = uRay[1];
  float1 alpha = uRay[2];

  float3 dir = float3(d, d, 0.0);
  float1 noise = Perlin::turb(dir + float3(time, 0.0, 62.1 + time) * 0.05,
                              float3(480.0, 320.0, 480.0), lacunarity, gain);
  noise = lerp(noise, 0.0, 0.3);
  // fade vertically
  float4 mist = float4(noise.xxx * (1.0 - coord.y), 1.0);
  // apply user alpha
  mist *= alpha;

  float4 color = texture(nr, coord);
  color += mist;

  return int4_to_icol(clamp(color, 0, 1) * 255);
}

inline uint32_t rgb_split(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double>& uRed = uniforms[0];
  const std::vector<double>& uGreen = uniforms[1];
  const std::vector<double>& uBlue = uniforms[2];

  float2 uv = float2(wh) / float2(nr.ncol(), nr.nrow());
  float1 r = texture(nr, uv + float2(uRed[0], uRed[1])).r;
  float1 g = texture(nr, uv + float2(uGreen[0], uGreen[1])).g;
  float1 b = texture(nr, uv + float2(uBlue[0], uBlue[1])).b;
  float1 a = texture(nr, uv).a;

  return int4_to_icol(float4(r, g, b, a) * 255);
}

inline uint32_t ripple(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double> uCenter = uniforms[0];  // vec2
  const std::vector<double> uFreq = uniforms[1];    // float
  const std::vector<double> uAmp = uniforms[2];     // float

  float2 resolution = float2(nr.ncol(), nr.nrow());
  float2 uv = float2(wh) / resolution;
  float2 center = float2(uCenter[0], uCenter[1]) / resolution;
  float1 distance = length(uv - center);
  float1 ripple = sin(distance * uFreq[0]) * uAmp[0];
  float2 tc = uv + ripple;
  if (tc.x > 1.0 || tc.y > 1.0 || tc.x < 0.0 || tc.y < 0.0) {
    const int4 finalColor = int4(0, 0, 0, 0);
    return int4_to_icol(finalColor);
  }
  float4 finalColor = texture(nr, tc) * 255;
  return int4_to_icol(finalColor);
}

inline uint32_t shockwave(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double> uCenter = uniforms[0];      // vec2
  const std::vector<double> uTime = uniforms[1];        // float
  const std::vector<double> uSpeed = uniforms[2];       // float
  const std::vector<double> uWave = uniforms[3];        // vec4
  const std::vector<double> uInputSize = uniforms[4];   // vec4
  const std::vector<double> uInputClamp = uniforms[5];  // vec4

  const float uAmplitude = uWave[0];
  const float uWavelength = uWave[1];
  const float uBrightness = uWave[2];
  const float uRadius = uWave[3];

  const float halfWavelength = uWavelength * 0.5 / max(uInputSize[0], uInputSize[1]);
  const float maxRadius = uRadius / max(uInputSize[0], uInputSize[1]);
  const float currentRadius = uTime[0] * uSpeed[0] / max(uInputSize[0], uInputSize[1]);

  const float2 vTextureCoord = float2(wh) / float2(nr.ncol(), nr.nrow());

  float1 fade = 1.0;

  if (maxRadius > 0.0) {
    if (currentRadius > maxRadius) {
      const int4 finalColor = texture_eval(nr, wh);
      return int4_to_icol(finalColor);
    }
    fade = 1.0 - pow(currentRadius / maxRadius, 2.0);
  }

  float2 dir =
      float2(vTextureCoord - float2(uCenter[0], uCenter[1]) / float2(uInputSize[0], uInputSize[1]));
  dir.y *= uInputSize[1] / uInputSize[0];
  float dist = length(dir);

  if (dist <= 0.0 || dist < currentRadius - halfWavelength ||
      dist > currentRadius + halfWavelength) {
    const int4 finalColor = texture_eval(nr, wh);
    return int4_to_icol(finalColor);
  }

  float2 diffUV = normalize(dir);

  float1 diff = (dist - currentRadius) / halfWavelength;

  float1 p = 1.0 - pow(abs(diff), 2.0);

  float1 powDiff = 1.25 * sin(diff * M_PI) * p * (uAmplitude * fade);

  float2 offset = diffUV * powDiff / float2(uInputSize[0], uInputSize[1]);

  float2 coord = vTextureCoord + offset;
  float2 clampedCoord = clamp(coord, float2(uInputClamp[0], uInputClamp[1]),
                              float2(uInputClamp[2], uInputClamp[3]));
  float4 color = texture(nr, clampedCoord);
  float1 len = length(coord - clampedCoord);
  if (len > float1(0.0)) {
    color *= max(float1(0.0), 1.0 - len);
  }

  float1 brightness = float1(1.0) + float1(uBrightness - 1.0) * p * fade;
  float4 finalColor = float4(color.rgb * brightness, color.a) * 255;

  return int4_to_icol(clamp(finalColor, 0, 255));
}

inline uint32_t twist(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double> uTwist = uniforms[0];      // vec2
  const std::vector<double> uOffset = uniforms[1];     // vec2
  const std::vector<double> uInputSize = uniforms[2];  // vec4

  const auto mapCoord = [&uInputSize](float2 coord) {
    coord *= float2(uInputSize[0], uInputSize[1]);  // (w, h)
    coord += float2(uInputSize[2], uInputSize[3]);  // (x offset, y offset)
    return coord;
  };
  const auto unmapCoord = [&uInputSize](float2 coord) {
    coord -= float2(uInputSize[2], uInputSize[3]);  // (x offset, y offset)
    coord /= float2(uInputSize[0], uInputSize[1]);  // (w, h)
    return coord;
  };
  const auto twistCoord = [&](float2 coord) {
    coord -= float2(uOffset[0], uOffset[1]);

    float dist = length(coord);
    float uRadius = uTwist[0];
    float uAngle = uTwist[1];

    if (dist < uRadius) {
      float ratioDist = (uRadius - dist) / uRadius;
      float angleMod = ratioDist * ratioDist * uAngle;
      float s = sin(angleMod);
      float c = cos(angleMod);
      coord = float2(coord.x * c - coord.y * s, coord.x * s + coord.y * c);
    }
    coord += float2(uOffset[0], uOffset[1]);

    return coord;
  };

  float2 coord = float2(wh) / float2(nr.ncol(), nr.nrow());
  coord = mapCoord(coord);
  coord = twistCoord(coord);
  coord = unmapCoord(coord);

  float4 col = texture(nr, coord);
  return int4_to_icol(clamp(col * 255, 0, 255));
}

}  // namespace Effects
