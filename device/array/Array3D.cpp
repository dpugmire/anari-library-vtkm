// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/Array3D.h"

namespace vtkm_device {

Array3D::Array3D(VTKmDeviceGlobalState *state, const Array3DMemoryDescriptor &d)
    : helium::Array3D(state, d)
{
  state->objectCounts.arrays++;
}

Array3D::~Array3D()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Array3D *);
