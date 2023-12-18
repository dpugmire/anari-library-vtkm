// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmDeviceGlobalState.h"
// helium
#include "helium/array/Array2D.h"

namespace vtkm_device {

using Array2DMemoryDescriptor = helium::Array2DMemoryDescriptor;

struct Array2D : public helium::Array2D
{
  Array2D(VTKmDeviceGlobalState *state, const Array2DMemoryDescriptor &d);
  ~Array2D() override;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Array2D *, ANARI_ARRAY2D);
