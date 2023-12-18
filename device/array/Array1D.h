// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmDeviceGlobalState.h"
// helium
#include "helium/array/Array1D.h"

namespace vtkm_device {

using Array1DMemoryDescriptor = helium::Array1DMemoryDescriptor;

struct Array1D : public helium::Array1D
{
  Array1D(VTKmDeviceGlobalState *state, const Array1DMemoryDescriptor &d);
  ~Array1D() override;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Array1D *, ANARI_ARRAY1D);
