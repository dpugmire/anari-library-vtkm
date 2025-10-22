// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "TransferFunction1D.h"
#include "array/ArrayConversion.h"
// Viskores
#include <viskores/cont/ArrayExtractComponent.h>
#include <viskores/cont/ArrayHandleConstant.h>
#include <viskores/cont/ArrayHandleStride.h>

namespace {

template <typename ComponentType>
void FillColorTable(
    viskores::cont::ColorTable &table, const viskores::cont::UnknownArrayHandle &array)
{
  viskores::Id numValues = array.GetNumberOfValues();

  std::array<viskores::cont::ArrayHandleStride<ComponentType>, 3> colorChannels;
  colorChannels[0] = array.ExtractComponent<ComponentType>(0);
  if (array.GetNumberOfComponentsFlat() > 1) {
    colorChannels[1] = array.ExtractComponent<ComponentType>(1);
  } else {
    colorChannels[1] = viskores::cont::ArrayExtractComponent(
        viskores::cont::ArrayHandleConstant<ComponentType>(0, numValues), 0);
  }
  if (array.GetNumberOfComponentsFlat() > 2) {
    colorChannels[2] = array.ExtractComponent<ComponentType>(2);
  } else {
    colorChannels[2] = viskores::cont::ArrayExtractComponent(
        viskores::cont::ArrayHandleConstant<ComponentType>(0, numValues), 0);
  }

  std::array<
      typename viskores::cont::ArrayHandleStride<ComponentType>::ReadPortalType,
      3>
      colorPortals;
  std::transform(colorChannels.begin(),
      colorChannels.end(),
      colorPortals.begin(),
      [](auto array) { return array.ReadPortal(); });
  if (numValues > 1) {
    viskores::Float64 scale = 1.0 / (numValues - 1);
    for (viskores::Id index = 0; index < numValues; ++index) {
      std::array<viskores::Float32, 3> color;
      std::transform(colorPortals.begin(),
          colorPortals.end(),
          color.begin(),
          [index](auto portal) {
            return static_cast<viskores::Float32>(portal.Get(index));
          });
      table.AddPoint(index * scale, {color[0], color[1], color[2]});
    }
  } else {
    // Special case: only one color given in array.
    std::array<viskores::Float32, 3> color;
    std::transform(colorPortals.begin(),
        colorPortals.end(),
        color.begin(),
        [](auto portal) { return static_cast<viskores::Float32>(portal.Get(0)); });
    table.AddPoint(0, {color[0], color[1], color[2]});
    table.AddPoint(1, {color[0], color[1], color[2]});
  }
}

} // namespace

namespace viskores_device {

TransferFunction1D::TransferFunction1D(ViskoresDeviceGlobalState *d)
    : Volume(d), m_spatialField(this), m_colorArray(this), m_opacityArray(this)
{}

void TransferFunction1D::commitParameters()
{
  this->Volume::commitParameters();

  this->m_spatialField = getParamObject<SpatialField>("value");

  this->m_unitDistance = getParam("unitDistance", 1.0f);

  this->m_colorArray = this->getParamObject<Array1D>("color");
  this->m_color = {1, 1, 1, 1};
  this->getParam("color", ANARI_FLOAT32_VEC4, &this->m_color);
  this->getParam("color", ANARI_FLOAT32_VEC3, &this->m_color);

  this->m_opacityArray = this->getParamObject<Array1D>("opacity");

  box1 range = {0, 1};
  this->getParam("valueRange", ANARI_FLOAT32_BOX1, &range);
  this->m_valueRange = {range.lower, range.upper};
}

void TransferFunction1D::finalize()
{
  Volume::finalize();

  if (!this->m_spatialField) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "'transferFunction1D' volume missing 'value' parameter");
    return;
  }

  // Reset and fill color table
  this->m_colorTable = viskores::cont::ColorTable(viskores::ColorSpace::Lab);
  if (this->m_colorArray) {
    // Convert to Viskores colors
    viskores::cont::UnknownArrayHandle vtkmColors =
        ANARIColorsToVTKmColors(this->m_colorArray->dataAsVTKmArray());

    // Copy colors into ColorTable
    // NOTE: I am not at all convinced that this is a good idea. If we are
    // getting an array, that probably means that the client has already sampled
    // the colors to the desired level, and we should just use that array.
    // Insetad, we are building peicewise linear segments and then resampling
    // again.
    if (vtkmColors.IsBaseComponentType<viskores::Float32>()) {
      FillColorTable<viskores::Float32>(this->m_colorTable, vtkmColors);
    } else if (vtkmColors.IsBaseComponentType<viskores::Float64>()) {
      FillColorTable<viskores::Float64>(this->m_colorTable, vtkmColors);
    }
  } else {
    this->m_colorTable.AddPoint(0, {m_color[0], m_color[1], m_color[2]});
    this->m_colorTable.AddPoint(1, {m_color[0], m_color[1], m_color[2]});
  }

  if (m_opacityArray) {
    if (m_opacityArray->size() > 1) {
      viskores::Float64 scale = 1.0 / (m_opacityArray->size() - 1);
      for (size_t index = 0; index < m_opacityArray->size(); ++index) {
        float opacity = *m_opacityArray->valueAt<float>(index);
        this->m_colorTable.AddPointAlpha(index * scale, opacity);
      }
    } else {
      float opacity = *m_opacityArray->valueAt<float>(0);
      this->m_colorTable.AddPointAlpha(0, opacity);
      this->m_colorTable.AddPointAlpha(1, opacity);
    }
  }

  this->m_colorTable.RescaleToRange(this->m_valueRange);

  viskores::cont::DataSet dataSet = this->m_spatialField->getDataSet();
  this->m_actor = std::make_shared<viskores::rendering::Actor>(dataSet.GetCellSet(),
      dataSet.GetCoordinateSystem(),
      dataSet.GetField("data"),
      this->m_colorTable);
  this->m_actor->SetScalarRange(this->m_colorTable.GetRange());
  this->m_mapper = std::make_shared<viskores::rendering::MapperVolume>();
}

const SpatialField *TransferFunction1D::spatialField() const
{
  return this->m_spatialField.get();
}

viskores::Bounds TransferFunction1D::bounds() const
{
  return isValid()
      ? this->m_spatialField->getDataSet().GetCoordinateSystem().GetBounds()
      : viskores::Bounds();
}

viskores::rendering::Actor *TransferFunction1D::actor() const
{
  return this->m_actor.get();
}

viskores::rendering::MapperVolume *TransferFunction1D::mapper() const
{
  return this->m_mapper.get();
}

bool TransferFunction1D::isValid() const
{
  return this->m_spatialField;
}

} // namespace viskores_device
