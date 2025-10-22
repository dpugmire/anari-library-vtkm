// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace viskores_device {

struct Orthographic : public Camera
{
  Orthographic(VTKmDeviceGlobalState *s);
  void commitParameters() override;
  void finalize() override;

  viskores::rendering::Camera camera(const viskores::Bounds &bounds) const override;

 private:
  viskores::Float32 m_aspect;
  viskores::Float32 m_height;
  viskores::Float32 m_near;
  viskores::Float32 m_far;
};

} // namespace viskores_device
