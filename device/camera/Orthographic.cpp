// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Orthographic.h"

namespace vtkm_device {

Orthographic::Orthographic(VTKmDeviceGlobalState *s) : Camera(s) {}

void Orthographic::commitParameters()
{
  Camera::commitParameters();

  this->m_aspect = this->getParam("aspect", vtkm::Float32(1));
  this->m_height = this->getParam("height", vtkm::Float32(1));
  this->m_near = this->getParam("near", vtkm::Float32(-1));
  this->m_far = this->getParam("far", vtkm::Float32(-1));
}

void Orthographic::finalize()
{
  Camera::finalize();
}

vtkm::rendering::Camera Orthographic::camera(const vtkm::Bounds &bounds) const
{
  // TODO: Implement orthographic projection correctly
  vtkm::rendering::Camera camera;

  vtkm::Vec3f_64 diagonal = { bounds.X.Length(), bounds.Y.Length(), bounds.Z.Length() };
  vtkm::Float32 length = static_cast<vtkm::Float32>(vtkm::Magnitude(diagonal));

  camera.SetPosition(this->m_position);
  camera.SetLookAt(this->m_position + (length * this->m_direction));
  camera.SetViewUp(this->m_up);
  camera.SetClippingRange(
      (this->m_near > 0) ? this->m_near : (0.001f * length),
      (this->m_far > 0) ? this->m_far : (100.f * length));
#if 0
  camera.SetViewport(this->m_imageRegion[0],
                     this->m_imageRegion[2],
                     this->m_imageRegion[1],
                     this->m_imageRegion[3]);
#endif

  return camera;
}

} // namespace vtkm_device
