// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmDeviceGlobalState.h"
// helium
#include "helium/array/Array3D.h"

namespace vtkm_device {

using Array3DMemoryDescriptor = helium::Array3DMemoryDescriptor;

struct Array3D : public helium::Array3D
{
  Array3D(VTKmDeviceGlobalState *state, const Array3DMemoryDescriptor &d);
  ~Array3D() override;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Array3D *, ANARI_ARRAY3D);
