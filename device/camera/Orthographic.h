// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace vtkm_device {

struct Orthographic : public Camera
{
  Orthographic(VTKmDeviceGlobalState *s);
  void commit() override;
};

} // namespace vtkm_device
