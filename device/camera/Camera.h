// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmTypes.h"
#include "../Object.h"
#include <vtkm/interop/anari/VtkmANARITypes.h>

#include <vtkm/rendering/Camera.h>

namespace vtkm_device {

struct Camera : public Object
{
  Camera(VTKmDeviceGlobalState *s);
  ~Camera() override;

  static Camera *createInstance(
      std::string_view type, VTKmDeviceGlobalState *state);

  virtual void commit() override;

  float4 imageRegion() const
  {
    return float4(this->m_imageRegion[0],
                  this->m_imageRegion[1],
                  this->m_imageRegion[2],
                  this->m_imageRegion[3]);
  }

  virtual vtkm::rendering::Camera camera(const vtkm::Bounds &bounds) const = 0;

 protected:
  vtkm::Vec3f_32 m_position;
  vtkm::Vec3f_32 m_direction;
  vtkm::Vec3f_32 m_up;
  vtkm::Vec4f_32 m_imageRegion;
};

struct UnknownCamera : public Camera
{
  UnknownCamera(VTKmDeviceGlobalState *s);

  vtkm::rendering::Camera camera(const vtkm::Bounds &) const override;

  bool isValid() const override;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Camera *, ANARI_CAMERA);
