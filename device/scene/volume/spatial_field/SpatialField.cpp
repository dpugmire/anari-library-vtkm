// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "SpatialField.h"
// Subtypes
#include "StructuredRegularField.h"

namespace viskores_device {

SpatialField::SpatialField(ViskoresDeviceGlobalState *s)
    : Object(ANARI_SPATIAL_FIELD, s)
{}

SpatialField::~SpatialField() = default;

SpatialField *SpatialField::createInstance(
    std::string_view subtype, ViskoresDeviceGlobalState *s)
{
  if (subtype == "structuredRegular") {
    return new StructuredRegularField(s);
  } else {
    return new UnknownSpatialField(s);
  }
}

UnknownSpatialField::UnknownSpatialField(ViskoresDeviceGlobalState *d)
    : SpatialField(d)
{}

void UnknownSpatialField::commitParameters()
{
  // invalid
}
void UnknownSpatialField::finalize()
{
  // invalid
}

bool UnknownSpatialField::isValid() const
{
  return false;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::SpatialField *);
