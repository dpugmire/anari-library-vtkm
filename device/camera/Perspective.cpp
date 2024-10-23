// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace vtkm_device {

Perspective::Perspective(VTKmDeviceGlobalState *s) : Camera(s) {}

void Perspective::commit()
{
  Camera::commit();

  this->camera.SetPosition(this->Position);
  this->camera.SetLookAt(this->Position + this->Dir);
  this->camera.SetViewUp(this->Up);
  this->camera.SetClippingRange(0.1f, 1e30f);
  this->camera.SetFieldOfView(
      anari::degrees(getParam("fovy", anari::radians(60.f))));
#if 0
  this->camera.SetViewport(this->ImageRegion[0],
                           this->ImageRegion[2],
                           this->ImageRegion[1],
                           this->ImageRegion[3]);
#endif
}

} // namespace vtkm_device
