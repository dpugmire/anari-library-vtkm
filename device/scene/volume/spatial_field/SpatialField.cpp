// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "SpatialField.h"

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
  return (SpatialField *)new UnknownObject(ANARI_SPATIAL_FIELD, s);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::SpatialField *);
