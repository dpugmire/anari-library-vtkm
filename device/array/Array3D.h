// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../ViskoresDeviceGlobalState.h"
// helium
#include "helium/array/Array3D.h"
// Viskores
#include <viskores/cont/UnknownArrayHandle.h>

namespace viskores_device {

using Array3DMemoryDescriptor = helium::Array3DMemoryDescriptor;

struct Array3D : public helium::Array3D
{
  Array3D(ViskoresDeviceGlobalState *state, const Array3DMemoryDescriptor &d);
  ~Array3D() override;

  void unmap() override;

         /// @brief Return the data for this array wrapped into a Viskores array handle.
         ///
         /// Note: Do not change the contents of the Viskores array handle. Although the
         /// data are in a Viskores array, it is still managed by ANARI, and changing the
         /// data outside of a map/unmap is forbidden.
  viskores::cont::UnknownArrayHandle dataAsViskoresArray() const;

 private:
  viskores::cont::UnknownArrayHandle m_ViskoresArray;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Array3D *, ANARI_ARRAY3D);
