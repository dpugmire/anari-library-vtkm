// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmDeviceGlobalState.h"
// helium
#include "helium/array/Array1D.h"
// VTK-m
#include <vtkm/cont/UnknownArrayHandle.h>

namespace vtkm_device {

using Array1DMemoryDescriptor = helium::Array1DMemoryDescriptor;

struct Array1D : public helium::Array1D
{
  Array1D(VTKmDeviceGlobalState *state, const Array1DMemoryDescriptor &d);
  ~Array1D() override;

  void unmap() override;

  /// @brief Return the data for this array wrapped into a VTK-m array handle.
  ///
  /// Note: Do not change the contents of the VTK-m array handle. Although the
  /// data are in a VTK-m array, it is still managed by ANARI, and changing the
  /// data outside of a map/unmap is forbidden.
  vtkm::cont::UnknownArrayHandle dataAsVTKmArray() const;

 private:
  vtkm::cont::UnknownArrayHandle m_VTKmArray;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Array1D *, ANARI_ARRAY1D);
