// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Array2D.h"
#include "ArrayConversion.h"

namespace viskores_device {

Array2D::Array2D(ViskoresDeviceGlobalState *state, const Array2DMemoryDescriptor &d)
    : helium::Array2D(state, d)
{
  state->objectCounts.arrays++;
}

Array2D::~Array2D()
{
  asViskoresDeviceState(deviceState())->objectCounts.arrays--;
}

void Array2D::unmap()
{
  this->helium::Array2D::unmap();
  // Invalidate Viskores ArrayHandle
  this->m_ViskoresArray.ReleaseResources();
}

viskores::cont::UnknownArrayHandle Array2D::dataAsViskoresArray() const
{
  if (!this->m_ViskoresArray.IsValid())
  {
    // Pull data from ANARI into Viskores.
    const_cast<Array2D *>(this)->m_ViskoresArray = ANARIArrayToViskoresArray(this);
  }

  return this->m_ViskoresArray;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Array2D *);
