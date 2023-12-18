// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Light.h"

namespace vtkm_device {

Light::Light(VTKmDeviceGlobalState *s) : Object(ANARI_LIGHT, s) {}

Light *Light::createInstance(std::string_view /*subtype*/, VTKmDeviceGlobalState *s)
{
  return (Light *)new UnknownObject(ANARI_LIGHT, s);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Light *);
