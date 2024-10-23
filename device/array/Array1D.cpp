// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Array1D.h"
#include "ArrayConversion.h"

namespace vtkm_device {

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
  // Invalidate VTK-m ArrayHandle
  this->m_VTKmArray.ReleaseResources();
}

vtkm::cont::UnknownArrayHandle Array1D::dataAsVTKmArray() const
{
  if (!this->m_VTKmArray.IsValid())
  {
    // Pull data from ANARI into VTK-m.
    const_cast<Array1D *>(this)->m_VTKmArray = ANARIArrayToVTKmArray(this);
  }

  return this->m_VTKmArray;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Array1D *);
