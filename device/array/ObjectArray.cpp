// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/ObjectArray.h"
#include "../VTKmDeviceGlobalState.h"

namespace viskores_device {

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

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::ObjectArray *);
