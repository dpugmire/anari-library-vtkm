// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
#include "array/Array1D.h"
//helium
#include <helium/utility/ChangeObserverPtr.h>

namespace vtkm_device {

struct Triangle : Geometry
{
  Triangle(VTKmDeviceGlobalState *s);

  void commit() override;

 private:
  helium::ChangeObserverPtr<Array1D> m_index;
  helium::ChangeObserverPtr<Array1D> m_vertexPosition;
  // TODO: Add other attributes to observe changes.
};

} // namespace vtkm_device
