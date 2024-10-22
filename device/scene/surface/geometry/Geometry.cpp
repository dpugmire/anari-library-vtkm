// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"
// subtypes
#include "Triangle.h"
// VTK-m
#include <vtkm/cont/Invoker.h>
#include <vtkm/worklet/WorkletMapField.h>
// C++
#include <limits>

namespace {

struct ConvertColorValues : vtkm::worklet::WorkletMapField
{
  using ControlSignature = void(FieldIn, FieldOut);
  template <typename InType, typename OutType>
  VTKM_EXEC void operator()(const InType& inValue, OutType& outValue) const
  {
    using InComponentType = typename InType::ComponentType;
    using OutComponentType = typename OutType::ComponentType;

    constexpr OutComponentType scale = OutComponentType{ 1 } /
        static_cast<OutComponentType>(std::numeric_limits<InComponentType>::max());
    for (vtkm::IdComponent index = 0; index < inValue.GetNumberOfComponents(); ++index)
    {
      outValue[index] = static_cast<OutComponentType>(inValue[index]) * scale;
    }
  }
};

template <typename T>
inline void FixColorsForType(vtkm::cont::UnknownArrayHandle& colorArray)
{
  using ArrayType = vtkm::cont::ArrayHandle<vtkm::Vec<T, 4>>;
  if (colorArray.CanConvert<ArrayType>()) {
    vtkm::cont::ArrayHandle<vtkm::Vec4f> retypedArray;
    vtkm::cont::Invoker invoke;
    invoke(ConvertColorValues{}, colorArray.AsArrayHandle<ArrayType>(), retypedArray);
    colorArray = retypedArray;
  }
}

} // anonymous namespace

namespace vtkm_device {

Geometry::Geometry(VTKmDeviceGlobalState *s) : Object(ANARI_GEOMETRY, s)
{
  s->objectCounts.geometries++;
}

Geometry::~Geometry()
{
  deviceState()->objectCounts.geometries--;
}

Geometry *Geometry::createInstance(
    std::string_view subtype, VTKmDeviceGlobalState *s)
{
  std::cout << "Creating geometry of type " << subtype << "\n";
  if (subtype == "triangle")
    return new Triangle(s);
  else
    return new UnknownGeometry(s);
}

void Geometry::AddAttributeInformation()
{
  for (std::string&& attribName : { "attribute0", "attribute1", "attribute2", "attribute3" }) {
    std::string paramName = "vertex." + attribName;
    if (this->hasParam(paramName))
      this->m_data.AddPointField(
          attribName, this->getParamObject<Array1D>(paramName)->dataAsVTKmArray());
  }

  if (this->hasParam("vertex.color")) {
    vtkm::cont::UnknownArrayHandle colorArray =
        this->getParamObject<Array1D>("vertex.color")->dataAsVTKmArray();
    // Colors can be either float or a fixed integer type. VTK-m only supports float colors.
    // If we get integer colors, convert them here.
    FixColorsForType<vtkm::UInt8>(colorArray);
    FixColorsForType<vtkm::UInt16>(colorArray);
    FixColorsForType<vtkm::UInt32>(colorArray);
    FixColorsForType<vtkm::UInt64>(colorArray);
    this->m_data.AddPointField("color", colorArray);
  }
}

UnknownGeometry::UnknownGeometry(VTKmDeviceGlobalState *s)
    : Geometry(s)
{
}

bool UnknownGeometry::isValid() const
{
  return false;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Geometry *);
