// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace vtkm_device {

struct Orthographic : public Camera
{
  Orthographic(VTKmDeviceGlobalState *s);
  void commitParameters() override;
  void finalize() override;

  vtkm::rendering::Camera camera(const vtkm::Bounds &bounds) const override;

 private:
  vtkm::Float32 m_aspect;
  vtkm::Float32 m_height;
  vtkm::Float32 m_near;
  vtkm::Float32 m_far;
};

} // namespace vtkm_device
