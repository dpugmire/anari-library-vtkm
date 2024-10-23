// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "SpatialField.h"
// Subtypes
#include "StructuredRegularField.h"

namespace vtkm_device {

SpatialField::SpatialField(VTKmDeviceGlobalState *s)
    : Object(ANARI_SPATIAL_FIELD, s)
{
  s->objectCounts.spatialFields++;
}

SpatialField::~SpatialField()
{
  deviceState()->objectCounts.spatialFields--;
}

SpatialField *SpatialField::createInstance(
    std::string_view subtype, VTKmDeviceGlobalState *s)
{
  if (subtype == "structuredRegular") {
    return new StructuredRegularField(s);
  } else {
    return new UnknownSpatialField(s);
  }
}

UnknownSpatialField::UnknownSpatialField(VTKmDeviceGlobalState *d) : SpatialField(d)
{
}

bool UnknownSpatialField::isValid() const
{
  return false;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::SpatialField *);
