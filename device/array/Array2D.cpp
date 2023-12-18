// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/Array2D.h"
#include "../VTKmDeviceGlobalState.h"

namespace vtkm_device {

Array2D::Array2D(VTKmDeviceGlobalState *state, const Array2DMemoryDescriptor &d)
    : helium::Array2D(state, d)
{
  state->objectCounts.arrays++;
}

Array2D::~Array2D()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Array2D *);
