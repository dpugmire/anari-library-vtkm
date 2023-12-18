// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmDeviceGlobalState.h"
#include "Array1D.h"
// helium
#include "helium/array/ObjectArray.h"

namespace vtkm_device {

struct ObjectArray : public helium::ObjectArray
{
  ObjectArray(VTKmDeviceGlobalState *state, const Array1DMemoryDescriptor &d);
  ~ObjectArray() override;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::ObjectArray *, ANARI_ARRAY1D);
