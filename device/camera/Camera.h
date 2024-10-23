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
    return float4(this->ImageRegion[0],
                  this->ImageRegion[1],
                  this->ImageRegion[2],
                  this->ImageRegion[3]);
  }

  vtkm::rendering::Camera GetCamera() const { return this->camera; }

 protected:
  vtkm::Vec3f_32 Position = vtkm::Vec3f_32(0.f,0.f,0.f);
  vtkm::Vec3f_32 Dir = vtkm::Vec3f_32(0.f,0.f,1.f);
  vtkm::Vec3f_32 Up = vtkm::Vec3f_32(0.f,1.f,0.f);
  vtkm::Vec4f_32 ImageRegion;

  vtkm::rendering::Camera camera;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Camera *, ANARI_CAMERA);
