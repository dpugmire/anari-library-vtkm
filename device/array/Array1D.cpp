// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Array1D.h"
#include "ArrayConversion.h"

namespace viskores_device {

Array1D::Array1D(VTKmDeviceGlobalState *state, const Array1DMemoryDescriptor &d)
    : helium::Array1D(state, d)
{
  state->objectCounts.arrays++;
}

Array1D::~Array1D()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

void Array1D::unmap()
{
  this->helium::Array1D::unmap();
  // Invalidate Viskores ArrayHandle
  this->m_VTKmArray.ReleaseResources();
}

viskores::cont::UnknownArrayHandle Array1D::dataAsVTKmArray() const
{
  if (!this->m_VTKmArray.IsValid())
  {
    // Pull data from ANARI into Viskores.
    const_cast<Array1D *>(this)->m_VTKmArray = ANARIArrayToVTKmArray(this);
  }

  return this->m_VTKmArray;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Array1D *);
