// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Material.h"

namespace vtkm_device {

Material::Material(VTKmDeviceGlobalState *s) : Object(ANARI_MATERIAL, s)
{
  s->objectCounts.materials++;
}

Material::~Material()
{
  deviceState()->objectCounts.materials--;
}

Material *Material::createInstance(
    std::string_view subtype, VTKmDeviceGlobalState *s)
{
  return (Material *)new UnknownObject(ANARI_MATERIAL, s);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Material *);
