// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"
// subtypes
#include "Triangle.h"

#include "array/ArrayConversion.h"

namespace vtkm_device {

Geometry::Geometry(VTKmDeviceGlobalState *s) : Object(ANARI_GEOMETRY, s) {}

Geometry::~Geometry() = default;

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
  for (std::string &&attribName :
      {"attribute0", "attribute1", "attribute2", "attribute3"}) {
    std::string paramName = "vertex." + attribName;
    if (this->hasParam(paramName))
      this->m_dataSet.AddPointField(attribName,
          this->getParamObject<Array1D>(paramName)->dataAsVTKmArray());
  }

  if (this->hasParam("vertex.color")) {
    vtkm::cont::UnknownArrayHandle colorArray =
        this->getParamObject<Array1D>("vertex.color")->dataAsVTKmArray();
    // Colors can be either float or a fixed integer type. VTK-m only supports
    // float colors. If we get integer colors, convert them here.
    this->m_dataSet.AddPointField("color", ANARIColorsToVTKmColors(colorArray));
  }
}

UnknownGeometry::UnknownGeometry(VTKmDeviceGlobalState *s) : Geometry(s) {}

void UnknownGeometry::commitParameters()
{
  // invalid
}

void UnknownGeometry::finalize()
{
  // invalid
}

bool UnknownGeometry::isValid() const
{
  return false;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Geometry *);
