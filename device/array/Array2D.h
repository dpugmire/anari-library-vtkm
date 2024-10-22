// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmDeviceGlobalState.h"
// helium
#include "helium/array/Array2D.h"
// VTK-m
#include <vtkm/cont/UnknownArrayHandle.h>

namespace vtkm_device {

using Array2DMemoryDescriptor = helium::Array2DMemoryDescriptor;

struct Array2D : public helium::Array2D
{
  Array2D(VTKmDeviceGlobalState *state, const Array2DMemoryDescriptor &d);
  ~Array2D() override;

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

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Array2D *, ANARI_ARRAY2D);
