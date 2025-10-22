// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
#include "sampler/Sampler.h"

namespace vtkm_device {

struct Material : public Object
{
  Material(VTKmDeviceGlobalState *s);
  ~Material() override;
  void commitParameters() override;
  void finalize() override;
  static Material *createInstance(
      std::string_view subtype, VTKmDeviceGlobalState *s);
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Material *, ANARI_MATERIAL);
