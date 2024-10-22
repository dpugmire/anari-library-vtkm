// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Volume.h"
#include "TransferFunction1D.h"


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
 if (subtype =="transferFunction1D")
   return new TransferFunction1D(s);
 else
   return new UnknownVolume(s);
}

void Volume::commit()
{
  m_id = getParam<uint32_t>("id", ~0u);
}

bool Volume::isValid() const
{
  return true;
}

UnknownVolume::UnknownVolume(VTKmDeviceGlobalState *d) : Volume(d)
{
}

box3 UnknownVolume::bounds() const
{
  return box3{};
}

vtkm::rendering::Actor *UnknownVolume::actor() const
{
  return nullptr;
}

vtkm::rendering::Mapper *UnknownVolume::mapper() const
{
  return nullptr;
}

bool UnknownVolume::isValid() const
{
  return false;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Volume *);
