// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Array2D.h"
#include "ArrayConversion.h"

namespace vtkm_device {

Array2D::Array2D(VTKmDeviceGlobalState *state, const Array2DMemoryDescriptor &d)
    : helium::Array2D(state, d)
{
  state->objectCounts.arrays++;
}

Array2D::~Array2D()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

void Array2D::unmap()
{
  this->helium::Array2D::unmap();
  // Invalidate VTK-m ArrayHandle
  this->m_VTKmArray.ReleaseResources();
}

vtkm::cont::UnknownArrayHandle Array2D::dataAsVTKmArray() const
{
  if (!this->m_VTKmArray.IsValid())
  {
    // Pull data from ANARI into VTK-m.
    const_cast<Array2D *>(this)->m_VTKmArray = ANARIArrayToVTKmArray(this);
  }

  return this->m_VTKmArray;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Array2D *);
