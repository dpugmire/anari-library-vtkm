// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/ObjectArray.h"
#include "../VTKmDeviceGlobalState.h"

namespace vtkm_device {

ObjectArray::ObjectArray(
    VTKmDeviceGlobalState *state, const Array1DMemoryDescriptor &d)
    : helium::ObjectArray(state, d)
{
  state->objectCounts.arrays++;
}

ObjectArray::~ObjectArray()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::ObjectArray *);
