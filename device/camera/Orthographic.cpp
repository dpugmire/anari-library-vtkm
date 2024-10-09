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

Ray Orthographic::createRay(const float2 &screen) const
{
  Ray ray;
  ray.dir = this->Dir;
  //ray.org = this->Pos00 + screen.x * this->PosDU + screen.y * this->PosDV;
  ray.org = this->Pos00 + screen[0] * this->PosDU + screen[1] * this->PosDV;
  return ray;
}

} // namespace vtkm_device
