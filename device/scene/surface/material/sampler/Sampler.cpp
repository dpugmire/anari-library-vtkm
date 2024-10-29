// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Sampler.h"

namespace vtkm_device {

Sampler::Sampler(VTKmDeviceGlobalState *s) : Object(ANARI_SAMPLER, s) {}

Sampler::~Sampler() = default;

Sampler *Sampler::createInstance(
    std::string_view subtype, VTKmDeviceGlobalState *s)
{
  return (Sampler *)new UnknownObject(ANARI_SAMPLER, s);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Sampler *);
