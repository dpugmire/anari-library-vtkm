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
    return (Camera *)new UnknownObject(ANARI_CAMERA, s);
}

void Camera::commit()
{
  auto pp = getParam<float3>("position", float3(0.f));
  auto dd = getParam<float3>("direction", float3(0.f, 0.f, 1.f));
  this->Position = getParam<vtkm::Vec3f_32>("position", vtkm::Vec3f_32(0.f));
  this->Dir = vtkm::Normal(
      getParam<vtkm::Vec3f_32>("direction", vtkm::Vec3f_32(0.f, 0.f, 1.f)));
  this->Up = vtkm::Normal(
      getParam<vtkm::Vec3f_32>("up", vtkm::Vec3f_32(0.f, 1.f, 0.f)));
  this->ImageRegion = vtkm::Vec4f_32(0.f, 0.f, 1.f, 1.f);
  getParam("imageRegion", ANARI_FLOAT32_BOX2, &this->ImageRegion);

  std::cout<<"Pos== "<<this->Position<<" pp= "<<pp<<std::endl;
  std::cout<<"Dir== "<<this->Dir<<" dd== "<<dd<<std::endl;
  std::cout<<"Up== "<<this->Up<<std::endl;


  markUpdated();
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Camera *);
