// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace vtkm_device {

struct Orthographic : public Camera
{
  Orthographic(VTKmDeviceGlobalState *s);

  void commit() override;

 private:
   vtkm::Vec3f_32 PosDU;
   vtkm::Vec3f_32 PosDV;
   vtkm::Vec3f_32 Pos00;
};

} // namespace vtkm_device

