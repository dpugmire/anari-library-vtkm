// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Sampler.h"

namespace viskores_device {

Sampler::Sampler(VTKmDeviceGlobalState *s) : Object(ANARI_SAMPLER, s) {}

Sampler::~Sampler() = default;

Sampler *Sampler::createInstance(
    std::string_view subtype, VTKmDeviceGlobalState *s)
{
  return (Sampler *)new UnknownObject(ANARI_SAMPLER, s);
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Sampler *);
