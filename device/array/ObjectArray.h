// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../ViskoresDeviceGlobalState.h"
#include "Array1D.h"
// helium
#include "helium/array/ObjectArray.h"

namespace viskores_device {

struct ObjectArray : public helium::ObjectArray
{
  ObjectArray(ViskoresDeviceGlobalState *state, const Array1DMemoryDescriptor &d);
  ~ObjectArray() override;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::ObjectArray *, ANARI_ARRAY1D);
