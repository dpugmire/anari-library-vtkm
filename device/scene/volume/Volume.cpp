// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Volume.h"
#include "TransferFunction1D.h"

namespace viskores_device {

Volume::Volume(ViskoresDeviceGlobalState *s) : Object(ANARI_VOLUME, s) {}

Volume::~Volume() = default;

Volume *Volume::createInstance(
    std::string_view subtype, ViskoresDeviceGlobalState *s)
{
  if (subtype == "transferFunction1D")
    return new TransferFunction1D(s);
  else
    return new UnknownVolume(s);
}

void Volume::commitParameters()
{
  m_id = getParam<uint32_t>("id", ~0u);
}

void Volume::finalize()
{
  // no-op
}

UnknownVolume::UnknownVolume(ViskoresDeviceGlobalState *d) : Volume(d) {}

viskores::Bounds UnknownVolume::bounds() const
{
  return viskores::Bounds{};
}

viskores::rendering::Actor *UnknownVolume::actor() const
{
  return nullptr;
}

viskores::rendering::Mapper *UnknownVolume::mapper() const
{
  return nullptr;
}

bool UnknownVolume::isValid() const
{
  return false;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Volume *);
