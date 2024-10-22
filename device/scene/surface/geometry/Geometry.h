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

  const vtkm::cont::DataSet &getData() const { return this->m_data; }

 protected:
  vtkm::cont::DataSet m_data;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Geometry *, ANARI_GEOMETRY);
