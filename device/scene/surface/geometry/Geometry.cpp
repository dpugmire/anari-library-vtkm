// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"

namespace vtkm_device {

Geometry::Geometry(VTKmDeviceGlobalState *s) : Object(ANARI_GEOMETRY, s)
{
  s->objectCounts.geometries++;
}

Geometry::~Geometry()
{
  deviceState()->objectCounts.geometries--;
}

Geometry *Geometry::createInstance(
    std::string_view subtype, VTKmDeviceGlobalState *s)
{
  return (Geometry *)new UnknownObject(ANARI_GEOMETRY, s);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Geometry *);
