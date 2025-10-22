// Copyright 2022-2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "ArrayConversion.h"
#include "anari/frontend/type_utility.h"
// Viskores
#include <viskores/cont/Invoker.h>
#include <viskores/worklet/WorkletMapField.h>
// C++
#include <limits>
#include <type_traits>


namespace {

template <typename BaseType, int NumComponents>
struct VTKmTypeImpl
{
  using type = viskores::Vec<BaseType, static_cast<viskores::IdComponent>(NumComponents)>;
};
template <typename BaseType>
struct VTKmTypeImpl<BaseType, 1>
{
  using type = BaseType;
};

template <int ANARIDataTypeId>
struct ANARIToVTKmTypeImpl
{
  using properties = anari::ANARITypeProperties<ANARIDataTypeId>;
  using type = typename VTKmTypeImpl<typename properties::base_type, properties::components>::type;
};

template <int ANARIDataTypeId>
using ANARIToVTKmType = typename ANARIToVTKmTypeImpl<ANARIDataTypeId>::type;

template <int ANARIDataTypeId>
struct ConstructArrayHandle
{
  using T = ANARIToVTKmType<ANARIDataTypeId>;
  viskores::cont::UnknownArrayHandle operator()(const void *memory, viskores::Id numValues)
  {
    return this->DoIt(memory, numValues, std::is_pointer<T>{});
  }

  viskores::cont::UnknownArrayHandle DoIt(
      const void *memory, viskores::Id numValues, std::false_type /*is_pointer*/)
  {
    // TODO: The ANARI interface generally passes arrays in the host memory space.
    // This can be really inefficient as data is pulled from GPU to CPU, passed to
    // ANARI, and then copied right back to the GPU. Need an extension to allow
    // client applications to pass device memory directly.
    return viskores::cont::make_ArrayHandle(
        reinterpret_cast<const T *>(memory), numValues, viskores::CopyFlag::Off);
  }

  viskores::cont::UnknownArrayHandle DoIt(const void *, viskores::Id, std::true_type /*is_pointer*/)
  {
    std::cout << "Cannot convert an array of type "
              << anari::ANARITypeProperties<ANARIDataTypeId>::type_name
              << " to Viskores.\n";
    return viskores::cont::UnknownArrayHandle{};
  }
};
template <>
struct ConstructArrayHandle<ANARI_UNKNOWN>
{
  viskores::cont::UnknownArrayHandle operator()(const void *, viskores::Id)
  {
    std::cout << "Cannot convert an array of type ANARI_UNKNOWN to Viskores.\n";
    return viskores::cont::UnknownArrayHandle{};
  }
};


struct ConvertColorValues : viskores::worklet::WorkletMapField
{
  using ControlSignature = void(FieldIn, FieldOut);
  template <typename InType, typename OutType>
  VISKORES_EXEC void operator()(const InType& inValue, OutType& outValue) const
  {
    using InComponentType = typename InType::ComponentType;
    using OutComponentType = typename OutType::ComponentType;

    constexpr OutComponentType scale = OutComponentType{ 1 } /
        static_cast<OutComponentType>(std::numeric_limits<InComponentType>::max());
    for (viskores::IdComponent index = 0; index < inValue.GetNumberOfComponents(); ++index)
    {
      outValue[index] = static_cast<OutComponentType>(inValue[index]) * scale;
    }
  }
};

template <typename T>
inline void FixColorsForType(viskores::cont::UnknownArrayHandle& colorArray)
{
  using ArrayType = viskores::cont::ArrayHandle<viskores::Vec<T, 4>>;
  if (colorArray.CanConvert<ArrayType>()) {
    viskores::cont::ArrayHandle<viskores::Vec4f> retypedArray;
    viskores::cont::Invoker invoke;
    invoke(ConvertColorValues{}, colorArray.AsArrayHandle<ArrayType>(), retypedArray);
    colorArray = retypedArray;
  }
}

} // anonymous namespace

namespace viskores_device {

viskores::cont::UnknownArrayHandle ANARIArrayToVTKmArray(const helium::Array *anariArray)
{
  viskores::Id numValues = anariArray->totalSize();
  const void *memory = anariArray->data();

  return anari::anariTypeInvoke<viskores::cont::UnknownArrayHandle, ConstructArrayHandle>(
      anariArray->elementType(), memory, numValues);
}

viskores::cont::UnknownArrayHandle ANARIColorsToVTKmColors(
    const viskores::cont::UnknownArrayHandle &anariColors)
{
  viskores::cont::UnknownArrayHandle vtkmColors = anariColors;
  FixColorsForType<viskores::UInt8>(vtkmColors);
  FixColorsForType<viskores::UInt16>(vtkmColors);
  FixColorsForType<viskores::UInt32>(vtkmColors);
  FixColorsForType<viskores::UInt64>(vtkmColors);
  return vtkmColors;
}

} // namespace viskores_device
