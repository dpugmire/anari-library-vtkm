// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"
// subtypes
#include "Triangle.h"

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
  std::cout << "Creating geometry of type " << subtype << "\n";
  if (subtype == "triangle")
    return new Triangle(s);
  else
    // This seems insanely dangerous.
    return (Geometry *)new UnknownObject(ANARI_GEOMETRY, s);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Geometry *);
