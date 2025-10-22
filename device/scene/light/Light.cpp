// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Light.h"

namespace viskores_device {

Light::Light(VTKmDeviceGlobalState *s) : Object(ANARI_LIGHT, s) {}

Light *Light::createInstance(std::string_view /*subtype*/, VTKmDeviceGlobalState *s)
{
  return (Light *)new UnknownObject(ANARI_LIGHT, s);
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Light *);
