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

  this->DirDU = vtkm::Normal(vtkm::Cross(this->Dir, this->Up)) * imgPlaneSize[0];
  this->DirDV = vtkm::Normal(vtkm::Cross(this->DirDU, this->Dir)) * imgPlaneSize[1];
  this->Dir00 = this->Dir - .5f * this->DirDU - .5f * this->DirDV;

  //std::cout<<"Dir: "<<this->Dir<<" Up: "<<this->Up<<std::endl;
  //std::cout<<"DirDU: "<<this->DirDU<<std::endl;
  //std::cout<<"DirDV: "<<this->DirDV<<std::endl;
  //std::cout<<"Dir00: "<<this->Dir00<<std::endl;

  this->camera.SetPosition(this->Position);
  this->camera.SetLookAt(this->Dir);
  this->camera.SetViewUp(this->Up);
  this->camera.SetViewport(this->ImageRegion[0],
                           this->ImageRegion[2],
                           this->ImageRegion[1],
                           this->ImageRegion[3]);
}

Ray Perspective::createRay(const float2 &screen) const
{
  Ray ray;
  ray.org = this->Position;
  //ray.dir = normalize(this->Dir_00 + screen.x * this->DirDU + screen.y * this->DirDV);
  ray.dir = vtkm::Normal(this->Dir00 + screen[0] * this->DirDU + screen[1] * this->DirDV);

  //std::cout<<"Ray: "<<ray.org<<" "<<this->DirDU<<" "<<this->DirDV<<" "<<this->Dir00<<std::endl;
  return ray;
}

} // namespace vtkm_device
