// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace vtkm_device {

Perspective::Perspective(VTKmDeviceGlobalState *s) : Camera(s) {}

void Perspective::commit()
{
  Camera::commit();

  // NOTE: demonstrate alternative 'raw' method for getting parameter values
  float fovy = 0.f;
  if (!getParam("fovy", ANARI_FLOAT32, &fovy))
    fovy = anari::radians(60.f);
  float aspect = getParam<float>("aspect", 1.f);

  vtkm::Vec2f_32 imgPlaneSize;
  imgPlaneSize[1] = 2.f * tanf(0.5f * fovy);
  imgPlaneSize[0] = imgPlaneSize[1] * aspect;

  this->DirDU =
      vtkm::Normal(vtkm::Cross(this->Dir, this->Up)) * imgPlaneSize[0];
  this->DirDV =
      vtkm::Normal(vtkm::Cross(this->DirDU, this->Dir)) * imgPlaneSize[1];
  this->Dir00 = this->Dir - .5f * this->DirDU - .5f * this->DirDV;
}

} // namespace vtkm_device
