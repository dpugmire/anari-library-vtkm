// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace vtkm_device {

struct Perspective : public Camera
{
  Perspective(VTKmDeviceGlobalState *s);

  void commit() override;
  Ray createRay(const float2 &screen) const override;

 private:
   vtkm::Vec3f_32 DirDU;
   vtkm::Vec3f_32 DirDV;
   vtkm::Vec3f_32 Dir00;
};

} // namespace vtkm_device
