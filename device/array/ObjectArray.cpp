// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/ObjectArray.h"
#include "../ViskoresDeviceGlobalState.h"

namespace viskores_device {

ObjectArray::ObjectArray(
    ViskoresDeviceGlobalState *state, const Array1DMemoryDescriptor &d)
    : helium::ObjectArray(state, d)
{
  state->objectCounts.arrays++;
}

ObjectArray::~ObjectArray()
{
  asViskoresDeviceState(deviceState())->objectCounts.arrays--;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::ObjectArray *);
