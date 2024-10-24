// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "TransferFunction1D.h"
#include "array/ArrayConversion.h"
// VTK-m
#include <vtkm/cont/ArrayExtractComponent.h>
#include <vtkm/cont/ArrayHandleConstant.h>
#include <vtkm/cont/ArrayHandleStride.h>

namespace {

template <typename ComponentType>
void FillColorTable(vtkm::cont::ColorTable& table, const vtkm::cont::UnknownArrayHandle& array)
{
  vtkm::Id numValues = array.GetNumberOfValues();

  std::array<vtkm::cont::ArrayHandleStride<ComponentType>, 3> colorChannels;
  colorChannels[0] = array.ExtractComponent<ComponentType>(0);
  if (array.GetNumberOfComponentsFlat() > 1) {
    colorChannels[1] = array.ExtractComponent<ComponentType>(1);
  } else {
    colorChannels[1] = vtkm::cont::ArrayExtractComponent(
        vtkm::cont::ArrayHandleConstant<ComponentType>(0, numValues), 0);
  }
  if (array.GetNumberOfComponentsFlat() > 2) {
    colorChannels[2] = array.ExtractComponent<ComponentType>(2);
  } else {
    colorChannels[2] = vtkm::cont::ArrayExtractComponent(
        vtkm::cont::ArrayHandleConstant<ComponentType>(0, numValues), 0);
  }

  std::array<typename vtkm::cont::ArrayHandleStride<ComponentType>::ReadPortalType, 3> colorPortals;
  std::transform(colorChannels.begin(), colorChannels.end(), colorPortals.begin(), [](auto array){
    return array.ReadPortal();
  });
  vtkm::Float64 scale = 1.0/(numValues - 1);
  for (vtkm::Id index = 0; index < numValues; ++index) {
    std::array<vtkm::Float32, 3> color;
    std::transform(colorPortals.begin(), colorPortals.end(), color.begin(), [index](auto portal){
      return static_cast<vtkm::Float32>(portal.Get(index));
    });
    table.AddPoint(index * scale, { color[0], color[1], color[2] });
  }
}

}

namespace vtkm_device {

TransferFunction1D::TransferFunction1D(VTKmDeviceGlobalState *d)
    : Volume(d), m_colorArray(this), m_opacity(this)
{
}

void TransferFunction1D::commit()
{
  this->Volume::commit();

  this->m_spatialField = getParamObject<SpatialField>("value");
  this->m_unitDistance = getParam("unitDistance", 1.0f);

  // Reset and fill color table
  this->m_colorTable = vtkm::cont::ColorTable(vtkm::ColorSpace::Lab);
  Array1D* colorArray = this->getParamObject<Array1D>("color");
  float4 color4;
  if (colorArray != nullptr) {
    // Convert to VTK-m colors
    vtkm::cont::UnknownArrayHandle vtkmColors =
        ANARIColorsToVTKmColors(colorArray->dataAsVTKmArray());

    // Copy colors into ColorTable
    // NOTE: I am not at all convinced that this is a good idea. If we are getting
    // an array, that probably means that the client has already sampled the colors
    // to the desired level, and we should just use that array. Insetad, we are building
    // peicewise linear segments and then resampling again.
    if (vtkmColors.IsBaseComponentType<vtkm::Float32>()) {
      FillColorTable<vtkm::Float32>(this->m_colorTable, vtkmColors);
    } else if (vtkmColors.IsBaseComponentType<vtkm::Float64>()) {
      FillColorTable<vtkm::Float64>(this->m_colorTable, vtkmColors);
    }
  } else if (this->getParam("color", ANARI_FLOAT32_VEC4, &color4)) {
    this->m_colorTable.AddPoint(0, { color4[0], color4[1], color4[2] });
    this->m_colorTable.AddPoint(1, { color4[0], color4[1], color4[2] });
  } else {
    float3 color3 = this->getParam("color", float3{ 1, 1, 1 });
    this->m_colorTable.AddPoint(0, { color3[0], color3[1], color3[2] });
    this->m_colorTable.AddPoint(1, { color3[0], color3[1], color3[2] });
  }

  Array1D* opacityArray = this->getParamObject<Array1D>("opacity");
  if (opacityArray != nullptr) {
    vtkm::Float64 scale = 1.0/(opacityArray->size() - 1);
    for (size_t index = 0; index < opacityArray->size(); ++index) {
      float opacity = *opacityArray->valueAt<float>(index);
      this->m_colorTable.AddPointAlpha(index * scale, opacity);
    }
  }

  box1 range = this->getParam("valueRange", box1{ 0, 1 });
  this->m_colorTable.RescaleToRange({ range.lower, range.upper });

  vtkm::cont::DataSet dataSet = this->m_spatialField->getDataSet();
  this->m_actor = std::make_shared<vtkm::rendering::Actor>(
      dataSet.GetCellSet(),
      dataSet.GetCoordinateSystem(),
      dataSet.GetField("data"),
      this->m_colorTable);
  this->m_mapper = std::make_shared<vtkm::rendering::MapperVolume>();
}

const SpatialField *TransferFunction1D::spatialField() const
{
  return this->m_spatialField.ptr;
}

vtkm::Bounds TransferFunction1D::bounds() const
{
  return this->m_spatialField->getDataSet().GetCoordinateSystem().GetBounds();
}

vtkm::rendering::Actor *TransferFunction1D::actor() const
{
  return this->m_actor.get();
}

vtkm::rendering::MapperVolume *TransferFunction1D::mapper() const
{
  return this->m_mapper.get();
}

} // namespace vtkm_device
