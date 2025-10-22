// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Array1D.h"
#include "ArrayConversion.h"

namespace viskores_device {

Array1D::Array1D(ViskoresDeviceGlobalState *state, const Array1DMemoryDescriptor &d)
    : helium::Array1D(state, d)
{
  state->objectCounts.arrays++;
}

Array1D::~Array1D()
{
  asViskoresDeviceState(deviceState())->objectCounts.arrays--;
}

void Array1D::unmap()
{
  this->helium::Array1D::unmap();
  // Invalidate Viskores ArrayHandle
  this->m_ViskoresArray.ReleaseResources();
}

viskores::cont::UnknownArrayHandle Array1D::dataAsViskoresArray() const
{
  if (!this->m_ViskoresArray.IsValid())
  {
    // Pull data from ANARI into Viskores.
    const_cast<Array1D *>(this)->m_ViskoresArray = ANARIArrayToViskoresArray(this);
  }

  return this->m_ViskoresArray;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Array1D *);
