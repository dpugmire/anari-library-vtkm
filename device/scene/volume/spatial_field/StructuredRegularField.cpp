// Copyright 2022-2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "StructuredRegularField.h"
// Viskores
#include <viskores/cont/ArrayCopy.h>
#include <viskores/cont/DataSetBuilderUniform.h>

namespace viskores_device {

StructuredRegularField::StructuredRegularField(VTKmDeviceGlobalState *d)
    : SpatialField(d), m_dataArray(this)
{
}

void StructuredRegularField::commitParameters()
{
  // Stashing these in a ChangeObserverPtr means that commit will be
  // called again if the array contents change.
  this->m_dataArray = getParamObject<Array3D>("data");

  float3 origin = getParam("origin", float3{ 0, 0, 0 });
  float3 spacing = getParam("spacing", float3{ 1, 1, 1 });
  uint3 dimensions = this->m_dataArray->size();

  this->m_dataSet = viskores::cont::DataSetBuilderUniform::Create(
      viskores::Id3{ dimensions[0], dimensions[1], dimensions[2] },
      viskores::Vec3f{ origin[0], origin[1], origin[2] },
      viskores::Vec3f{ spacing[0], spacing[1], spacing[2] });
}

void StructuredRegularField::finalize()
{
  // Viskores volume render only supports float fields in volume rendering. Convert
  // if necessary.
  viskores::cont::UnknownArrayHandle vtkmArray = this->m_dataArray->dataAsVTKmArray();
  if (!vtkmArray.IsValueType<viskores::Float32>() && !vtkmArray.IsValueType<viskores::Float64>())
  {
    viskores::cont::ArrayHandle<viskores::FloatDefault> castArray;
    viskores::cont::ArrayCopy(vtkmArray, castArray);
    vtkmArray = castArray;
  }
  this->m_dataSet.AddPointField("data", vtkmArray);

  this->m_dataSet.PrintSummary(std::cout);
}

} // namespace viskores_device
