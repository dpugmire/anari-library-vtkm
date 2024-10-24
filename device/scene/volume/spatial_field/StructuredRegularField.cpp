// Copyright 2022-2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "StructuredRegularField.h"
// VTK-m
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/DataSetBuilderUniform.h>

namespace vtkm_device {

StructuredRegularField::StructuredRegularField(VTKmDeviceGlobalState *d)
    : SpatialField(d), m_dataArray(this)
{
}

void StructuredRegularField::commit()
{
  // Stashing these in a ChangeObserverPtr means that commit will be
  // called again if the array contents change.
  this->m_dataArray = getParamObject<Array3D>("data");

  float3 origin = getParam("origin", float3{ 0, 0, 0 });
  float3 spacing = getParam("spacing", float3{ 1, 1, 1 });
  uint3 dimensions = this->m_dataArray->size();

  this->m_dataSet = vtkm::cont::DataSetBuilderUniform::Create(
      vtkm::Id3{ dimensions[0], dimensions[1], dimensions[2] },
      vtkm::Vec3f{ origin[0], origin[1], origin[2] },
      vtkm::Vec3f{ spacing[0], spacing[1], spacing[2] });

  // VTK-m volume render only supports float fields in volume rendering. Convert
  // if necessary.
  vtkm::cont::UnknownArrayHandle vtkmArray = this->m_dataArray->dataAsVTKmArray();
  if (!vtkmArray.IsValueType<vtkm::Float32>() && !vtkmArray.IsValueType<vtkm::Float64>())
  {
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> castArray;
    vtkm::cont::ArrayCopy(vtkmArray, castArray);
    vtkmArray = castArray;
  }
  this->m_dataSet.AddPointField("data", vtkmArray);

  this->m_dataSet.PrintSummary(std::cout);
}

} // namespace vtkm_device
