// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Camera.h"
// specific types
#include "Orthographic.h"
#include "Perspective.h"

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
  if (type == "perspective")
    return new Perspective(s);
  else if (type == "orthographic")
    return new Orthographic(s);
  else
    return new UnknownCamera(s);
}

void Camera::commit()
{
  this->m_position = getParam<vtkm::Vec3f_32>("position", { 0.f, 0.f, 0.f });
  this->m_direction = vtkm::Normal(getParam<vtkm::Vec3f_32>("direction", { 0.f, 0.f, -1.f }));
  this->m_up = vtkm::Normal(getParam<vtkm::Vec3f_32>("up", { 0.f, 1.f, 0.f }));
  this->m_imageRegion = vtkm::Vec4f_32(0.f, 0.f, 1.f, 1.f);
  getParam("imageRegion", ANARI_FLOAT32_BOX2, &this->m_imageRegion);

  std::cout<<"Pos== "<<this->m_position <<std::endl;
  std::cout<<"Dir== "<<this->m_direction <<std::endl;
  std::cout<<"Up== "<<this->m_up <<std::endl;

  markUpdated();
}

UnknownCamera::UnknownCamera(VTKmDeviceGlobalState *s) : Camera(s) {};

vtkm::rendering::Camera UnknownCamera::camera(const vtkm::Bounds&) const
{
  return {};
}

bool UnknownCamera::isValid() const
{
  return false;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Camera *);
