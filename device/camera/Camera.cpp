// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Camera.h"

namespace vtkm_device {

Camera::Camera(VTKmDeviceGlobalState *s) : Object(ANARI_CAMERA, s)
{
  s->objectCounts.cameras++;
}

Camera::~Camera()
{
  deviceState()->objectCounts.cameras--;
}

Camera *Camera::createInstance(std::string_view type, VTKmDeviceGlobalState *s)
{
  return (Camera *)new UnknownObject(ANARI_CAMERA, s);
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Camera *);
