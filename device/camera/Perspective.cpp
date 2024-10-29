// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace vtkm_device {

Perspective::Perspective(VTKmDeviceGlobalState *s) : Camera(s) {}

void Perspective::commit()
{
  Camera::commit();

  this->m_fovy = this->getParam("fovy", vtkm::Pi_3f());
  this->m_aspect = this->getParam("aspect", vtkm::Float32(1));
  this->m_near = this->getParam("near", vtkm::Float32(-1));
  this->m_far = this->getParam("far", vtkm::Float32(-1));
}

vtkm::rendering::Camera Perspective::camera(const vtkm::Bounds &bounds) const
{
  vtkm::rendering::Camera camera;

  vtkm::Vec3f_64 diagonal = { bounds.X.Length(), bounds.Y.Length(), bounds.Z.Length() };
  vtkm::Float32 length = static_cast<vtkm::Float32>(vtkm::Magnitude(diagonal));

  camera.SetPosition(this->m_position);
  camera.SetLookAt(this->m_position + (length * this->m_direction));
  camera.SetViewUp(this->m_up);
  camera.SetFieldOfView(anari::degrees(this->m_fovy));
  camera.SetClippingRange(
      (this->m_near > 0) ? this->m_near : (0.01f * length),
      (this->m_far > 0) ? this->m_far : (1000.f * length));
#if 0
  camera.SetViewport(this->m_imageRegion[0],
                     this->m_imageRegion[2],
                     this->m_imageRegion[1],
                     this->m_imageRegion[3]);
#endif

  // TODO: The aspect parameter is ignored. This is handled elsewhere

  return camera;
}

} // namespace vtkm_device
