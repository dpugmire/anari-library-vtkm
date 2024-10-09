// Copyright 2022-2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <limits>

// helium
//#include "helium/helium_math.h"
// embree
//#include <embree3/rtcore_common.h>

#if 1
namespace helide {

//using namespace anari::math;
//using namespace helium::math;

using float2 = float[2];
using float3 = float[3];
using float4 = float[4];

// from helium_math.h
template <typename T>
struct range_t
{
  using element_t = T;

  range_t() = default;
  range_t(const T &t) : lower(t), upper(t) {}
  range_t(const T &_lower, const T &_upper) : lower(_lower), upper(_upper) {}

  range_t<T> &extend(const T &t)
  {
    lower = min(lower, t);
    upper = max(upper, t);
    return *this;
  }

  range_t<T> &extend(const range_t<T> &t)
  {
    lower = min(lower, t.lower);
    upper = max(upper, t.upper);
    return *this;
  }

  T lower{T(std::numeric_limits<float>::max())};
  T upper{T(-std::numeric_limits<float>::max())};
};


using box1 = range_t<float>;
using box2 = range_t<float2>;
using box3 = range_t<float3>;
using uint32_t = unsigned int;

struct Ray
{
  // Ray //

  float3 org;
  float tnear{0.f};
  float3 dir;
  float time{0.f};
  float tfar{std::numeric_limits<float>::max()};
  unsigned int mask{~0u};
  unsigned int id{0};
  unsigned int flags{0};

  // Hit //

  float3 Ng;
  float u;
  float v;
  unsigned int primID; //{RTC_INVALID_GEOMETRY_ID}; // primitive ID
  unsigned int geomID; //{RTC_INVALID_GEOMETRY_ID}; // geometry ID
  unsigned int instID; //{RTC_INVALID_GEOMETRY_ID}; // instance ID
};

struct Volume;
struct VolumeRay
{
  float3 org;
  float3 dir;
  box1 t{0.f, std::numeric_limits<float>::max()};
  Volume *volume{nullptr};
  uint32_t instID; //{RTC_INVALID_GEOMETRY_ID};
};

} // namespace helide

#endif
