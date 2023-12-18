// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/Array1D.h"

namespace vtkm_device {

Array1D::Array1D(VTKmDeviceGlobalState *state, const Array1DMemoryDescriptor &d)
    : helium::Array1D(state, d)
{
  state->objectCounts.arrays++;
}

Array1D::~Array1D()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Array1D *);
