// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace vtkm_device {

struct Light : public Object
{
  Light(VTKmDeviceGlobalState *d);
  static Light *createInstance(std::string_view subtype, VTKmDeviceGlobalState *d);
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Light *, ANARI_LIGHT);
