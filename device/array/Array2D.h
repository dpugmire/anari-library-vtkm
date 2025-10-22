// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../ViskoresDeviceGlobalState.h"
// helium
#include "helium/array/Array2D.h"
// Viskores
#include <viskores/cont/UnknownArrayHandle.h>

namespace viskores_device {

using Array2DMemoryDescriptor = helium::Array2DMemoryDescriptor;

struct Array2D : public helium::Array2D
{
  Array2D(ViskoresDeviceGlobalState *state, const Array2DMemoryDescriptor &d);
  ~Array2D() override;

  void unmap() override;

         /// @brief Return the data for this array wrapped into a Viskores array handle.
         ///
         /// Note: Do not change the contents of the Viskores array handle. Although the
         /// data are in a Viskores array, it is still managed by ANARI, and changing the
         /// data outside of a map/unmap is forbidden.
  viskores::cont::UnknownArrayHandle dataAsVTKmArray() const;

 private:
  viskores::cont::UnknownArrayHandle m_VTKmArray;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Array2D *, ANARI_ARRAY2D);
