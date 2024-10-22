// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

#include <vtkm/cont/DataSet.h>

namespace vtkm_device {

struct Geometry : public Object
{
  Geometry(VTKmDeviceGlobalState *s);
  ~Geometry() override;
  static Geometry *createInstance(
      std::string_view subtype, VTKmDeviceGlobalState *s);

  const vtkm::cont::DataSet &getDataSet() const { return this->m_dataSet; }

 protected:
  vtkm::cont::DataSet m_dataSet;

  /// @brief A convenience method to pull attribute information into m_data.
  ///
  /// This method looks for the properties named `vertex.color` and
  /// `vertex.attribute[0-3]`. For any that exist, they will be converted
  /// into fields and placed in the `m_data` object. They will be given
  /// the field names `color` and `attribute[0-3]`, respectively, which
  /// matches the attribute strings specified by ANARI.
  ///
  /// If any of these properties are not provided, no such respective
  /// array will be added as a field to the VTK-m data object.
  void AddAttributeInformation();
};

struct UnknownGeometry : public Geometry
{
  UnknownGeometry(VTKmDeviceGlobalState *s);
  bool isValid() const override;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Geometry *, ANARI_GEOMETRY);
