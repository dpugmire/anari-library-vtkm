// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace viskores_device {

struct Geometry;

struct Sampler : public Object
{
  Sampler(ViskoresDeviceGlobalState *d);
  virtual ~Sampler();
  static Sampler *createInstance(
      std::string_view subtype, ViskoresDeviceGlobalState *d);
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Sampler *, ANARI_SAMPLER);
