// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Array3D.h"
#include "ArrayConversion.h"

namespace viskores_device {

Array3D::Array3D(ViskoresDeviceGlobalState *state, const Array3DMemoryDescriptor &d)
    : helium::Array3D(state, d)
{
  state->objectCounts.arrays++;
}

Array3D::~Array3D()
{
  asViskoresDeviceState(deviceState())->objectCounts.arrays--;
}

void Array3D::unmap()
{
  this->helium::Array3D::unmap();
  // Invalidate Viskores ArrayHandle
  this->m_VTKmArray.ReleaseResources();
}

viskores::cont::UnknownArrayHandle Array3D::dataAsVTKmArray() const
{
  if (!this->m_VTKmArray.IsValid())
  {
    // Pull data from ANARI into Viskores.
    const_cast<Array3D *>(this)->m_VTKmArray = ANARIArrayToVTKmArray(this);
  }

  return this->m_VTKmArray;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Array3D *);
