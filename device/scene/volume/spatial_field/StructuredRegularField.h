// Copyright 2022-2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SpatialField.h"
#include "array/Array3D.h"
// helium
#include <helium/utility/ChangeObserverPtr.h>

namespace vtkm_device
{

struct StructuredRegularField : public SpatialField
{
  StructuredRegularField(VTKmDeviceGlobalState *d);

  void commitParameters() override;
  void finalize() override;

 private:
  helium::ChangeObserverPtr<Array3D> m_dataArray;
};

} // namespace vtkm_device
