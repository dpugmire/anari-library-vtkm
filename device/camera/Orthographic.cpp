// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Orthographic.h"

namespace vtkm_device {

Orthographic::Orthographic(VTKmDeviceGlobalState *s) : Camera(s) {}

void Orthographic::commit()
{
  Camera::commit();

  const float aspect = getParam<float>("aspect", 1.f);
  const float height = getParam<float>("height", 1.f);

  vtkm::Vec2f_32 imgPlaneSize(height * aspect, height);

  this->PosDU =
      vtkm::Normal(vtkm::Cross(this->Dir, this->Up)) * imgPlaneSize[0];
  this->PosDV =
      vtkm::Normal(vtkm::Cross(this->PosDU, this->Dir)) * imgPlaneSize[1];
  this->Pos00 = this->Position - .5f * this->PosDU - .5f * this->PosDV;
}

} // namespace vtkm_device
