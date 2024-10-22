// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Array3D.h"
#include "ArrayConversion.h"

namespace vtkm_device {

Array3D::Array3D(VTKmDeviceGlobalState *state, const Array3DMemoryDescriptor &d)
    : helium::Array3D(state, d)
{
  state->objectCounts.arrays++;
}

Array3D::~Array3D()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

void Array3D::unmap()
{
  this->helium::Array3D::unmap();
  // Invalidate VTK-m ArrayHandle
  this->m_VTKmArray.ReleaseResources();
}

vtkm::cont::UnknownArrayHandle Array3D::dataAsVTKmArray() const
{
  if (!this->m_VTKmArray.IsValid())
  {
    // Pull data from ANARI into VTK-m.
    const_cast<Array3D *>(this)->m_VTKmArray = ANARIArrayToVTKmArray(this);
  }

  return this->m_VTKmArray;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Array3D *);
