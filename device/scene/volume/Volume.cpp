// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Volume.h"

namespace vtkm_device {

Volume::Volume(VTKmDeviceGlobalState *s) : Object(ANARI_VOLUME, s)
{
  s->objectCounts.volumes++;
}

Volume::~Volume()
{
  deviceState()->objectCounts.volumes--;
}

Volume *Volume::createInstance(std::string_view subtype, VTKmDeviceGlobalState *s)
{
  return (Volume *)new UnknownObject(ANARI_VOLUME, s);
}

void Volume::commit()
{
  m_id = getParam<uint32_t>("id", ~0u);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Volume *);
