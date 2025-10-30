#include "Image1DSampler.h"
// Viskores
#include <viskores/TypeTraits.h>
#include <viskores/cont/ArrayExtractComponent.h>
#include <viskores/cont/ArrayHandleConstant.h>
// std
#include <limits>

namespace {

template <typename T>
constexpr viskores::Float32 floatConvert(
    T anariValue, viskores::TypeTraitsRealTag)
{
  return static_cast<viskores::Float32>(anariValue);
}
template <typename T>
constexpr viskores::Float32 floatConvert(
    T anariValue, viskores::TypeTraitsIntegerTag)
{
  return static_cast<viskores::Float32>(anariValue)
      / static_cast<viskores::Float32>(std::numeric_limits<T>::max());
}
template <typename T>
constexpr viskores::Float32 floatConvert(T anariValue)
{
  return floatConvert(
      anariValue, typename viskores::TypeTraits<T>::NumericTag{});
}

template <typename ComponentType>
void captureColorTableType(const viskores::cont::UnknownArrayHandle &colorArray,
    viskores::cont::ColorTable &colorTable)
{
  const viskores::Id numValues = colorArray.GetNumberOfValues();
  std::array<viskores::cont::ArrayHandleStride<ComponentType>, 4>
      colorChannelArrays;
  std::array<
      typename viskores::cont::ArrayHandleStride<ComponentType>::ReadPortalType,
      4>
      colorChannelPortals;
  for (viskores::IdComponent channel = 0; channel < 4; ++channel) {
    if (channel < colorArray.GetNumberOfComponentsFlat()) {
      colorChannelArrays[channel] =
          colorArray.ExtractComponent<ComponentType>(channel);
    } else {
      colorChannelArrays[channel] = viskores::cont::ArrayExtractComponent(
          viskores::cont::ArrayHandleConstant<ComponentType>(0, numValues),
          channel);
    }
    colorChannelPortals[channel] = colorChannelArrays[channel].ReadPortal();
  }
  ComponentType sampleDelta = 1.0f / ComponentType(numValues);
  for (viskores::Id sample = 0; sample < numValues; ++sample) {
    viskores::Vec3f_32 color{floatConvert(colorChannelPortals[0].Get(sample)),
        floatConvert(colorChannelPortals[1].Get(sample)),
        floatConvert(colorChannelPortals[2].Get(sample))};
    colorTable.AddPoint(sample * sampleDelta, color);
    ComponentType alpha = floatConvert(colorChannelPortals[3].Get(sample));
    colorTable.AddPointAlpha(sample * sampleDelta, alpha);
  }
}

bool captureColorTable(const viskores::cont::UnknownArrayHandle &colorArray,
    viskores::cont::ColorTable &colorTable)
{
  if (colorArray.IsBaseComponentType<viskores::UInt8>()) {
    captureColorTableType<viskores::UInt8>(colorArray, colorTable);
    return true;
  } else if (colorArray.IsBaseComponentType<viskores::UInt16>()) {
    captureColorTableType<viskores::UInt16>(colorArray, colorTable);
    return true;
  } else if (colorArray.IsBaseComponentType<viskores::UInt32>()) {
    captureColorTableType<viskores::UInt16>(colorArray, colorTable);
    return true;
  } else if (colorArray.IsBaseComponentType<viskores::Float32>()) {
    captureColorTableType<viskores::UInt16>(colorArray, colorTable);
    return true;
  } else {
    return false;
  }
}

} // anonymous namespace
namespace viskores_device {

Image1DSampler::Image1DSampler(ViskoresDeviceGlobalState *d)
    : Sampler(d), m_colorArray(this)
{}

void Image1DSampler::commitParameters()
{
  this->Sampler::commitParameters();

  this->m_inAttribute = this->getParamString("inAttribute", "attribute0");

  mat4 inTransform = this->getParam("inTransform", mat4(linalg::identity));
  this->m_inTransform = toViskoresMatrix(inTransform);
  float4 inOffset = this->getParam("inOffset", float4(0.f, 0.f, 0.f, 0.f));
  this->m_inOffset = {inOffset[0], inOffset[1], inOffset[2], inOffset[3]};

  this->m_colorArray = this->getParamObject<Array1D>("image");

  this->m_wrapMode = helium::wrapModeFromString(
      this->getParamString("wrapMode", "clampToEdge"));
  if (this->m_wrapMode == helium::WrapMode::DEFAULT) {
    this->m_wrapMode = helium::WrapMode::CLAMP_TO_EDGE;
  }
}

void Image1DSampler::finalize()
{
  this->Sampler::finalize();

  // TODO: Viskores' ColorTable is not the best place to put a texture
  // because it just gets resampled. We will have to crack open the
  // rendering components to implement something like that.
  this->m_colorTable = viskores::cont::ColorTable{viskores::ColorSpace::RGB};
  bool colorTableFilled = false;
  if (this->m_colorArray) {
    // TODO: This method does not properly handle vec2 nor SRGB.
    colorTableFilled = captureColorTable(this->m_colorArray->dataAsViskoresArray(), this->m_colorTable);
    if (!colorTableFilled) {
      this->reportMessage(ANARI_SEVERITY_WARNING, "color array provided for image1D sampling has unrecognized type");
    }
  }

  if (!colorTableFilled) {
    this->reportMessage(ANARI_SEVERITY_WARNING,
        "image1D sampling requested, but no color array given");
    this->m_colorTable.AddPoint(0, {1, 1, 1});
  }
}

} // namespace viskores_device
