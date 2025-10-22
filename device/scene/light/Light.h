// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace viskores_device {

struct Light : public Object
{
  Light(VTKmDeviceGlobalState *d);
  static Light *createInstance(std::string_view subtype, VTKmDeviceGlobalState *d);
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Light *, ANARI_LIGHT);
