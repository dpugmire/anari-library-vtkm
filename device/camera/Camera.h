// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"

namespace vtkm_device {

struct Camera : public Object
{
  Camera(VTKmDeviceGlobalState *s);
  ~Camera() override;

  static Camera *createInstance(
      std::string_view type, VTKmDeviceGlobalState *state);

  virtual void commit() override;

 protected:
  vtkm::Vec3f_32 Position;
  vtkm::Vec3f_32 Dir;
  vtkm::Vec3f_32 Up;
  vtkm::Vec4f_32 ImageRegion;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Camera *, ANARI_CAMERA);
