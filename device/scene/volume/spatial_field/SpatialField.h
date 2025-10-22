// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
// Viskores
#include <viskores/cont/DataSet.h>

namespace viskores_device {

struct SpatialField : public Object
{
  SpatialField(VTKmDeviceGlobalState *d);
  virtual ~SpatialField();
  static SpatialField *createInstance(
      std::string_view subtype, VTKmDeviceGlobalState *d);

  viskores::cont::DataSet getDataSet() const { return this->m_dataSet; }

 protected:
  viskores::cont::DataSet m_dataSet;
};

struct UnknownSpatialField : public SpatialField
{
  UnknownSpatialField(VTKmDeviceGlobalState *d);

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(
    viskores_device::SpatialField *, ANARI_SPATIAL_FIELD);
