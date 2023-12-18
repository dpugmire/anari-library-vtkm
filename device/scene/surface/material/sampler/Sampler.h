// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace vtkm_device {

struct Geometry;

struct Sampler : public Object
{
  Sampler(VTKmDeviceGlobalState *d);
  virtual ~Sampler();
  static Sampler *createInstance(
      std::string_view subtype, VTKmDeviceGlobalState *d);
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Sampler *, ANARI_SAMPLER);
