// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

#include <viskores/cont/DataSet.h>
#include <viskores/rendering/Actor.h>
#include <viskores/rendering/MapperRayTracer.h>

namespace viskores_device {

struct Geometry : public Object
{
  Geometry(ViskoresDeviceGlobalState *s);
  ~Geometry() override;
  static Geometry *createInstance(
      std::string_view subtype, ViskoresDeviceGlobalState *s);

  virtual viskores::rendering::Actor *actor() const
  {
    return nullptr;
  } // = 0;
  const viskores::cont::DataSet &getDataSet() const
  {
    return this->m_dataSet;
  }
  virtual const viskores::rendering::Mapper *mapper() const
  {
    return this->m_mapper.get();
  }

 protected:
  viskores::cont::DataSet m_dataSet;
  std::shared_ptr<viskores::rendering::Mapper> m_mapper;

  /// @brief A convenience method to pull attribute information into m_data.
  ///
  /// This method looks for the properties named `vertex.color` and
  /// `vertex.attribute[0-3]`. For any that exist, they will be converted
  /// into fields and placed in the `m_data` object. They will be given
  /// the field names `color` and `attribute[0-3]`, respectively, which
  /// matches the attribute strings specified by ANARI.
  ///
  /// If any of these properties are not provided, no such respective
  /// array will be added as a field to the Viskores data object.
  void AddAttributeInformation();
};

struct UnknownGeometry : public Geometry
{
  UnknownGeometry(ViskoresDeviceGlobalState *s);
  void commitParameters() override;
  void finalize() override;
  bool isValid() const override;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(
    viskores_device::Geometry *, ANARI_GEOMETRY);
