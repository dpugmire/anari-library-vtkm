// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../ViskoresDeviceGlobalState.h"
// helium
#include "helium/array/Array1D.h"
// Viskores
#include <viskores/cont/UnknownArrayHandle.h>

namespace viskores_device {

using Array1DMemoryDescriptor = helium::Array1DMemoryDescriptor;

struct Array1D : public helium::Array1D
{
  Array1D(ViskoresDeviceGlobalState *state, const Array1DMemoryDescriptor &d);
  ~Array1D() override;

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

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Array1D *, ANARI_ARRAY1D);
