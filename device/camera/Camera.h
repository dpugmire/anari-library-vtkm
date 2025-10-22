// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../VTKmTypes.h"
#include "../Object.h"
#include <viskores/interop/anari/ViskoresANARITypes.h>

#include <viskores/rendering/Camera.h>

namespace viskores_device {

struct Camera : public Object
{
  Camera(VTKmDeviceGlobalState *s);
  ~Camera() override;

  static Camera *createInstance(
      std::string_view type, VTKmDeviceGlobalState *state);

  virtual void commitParameters() override;
  virtual void finalize() override;

  float4 imageRegion() const
  {
    return float4(this->m_imageRegion[0],
                  this->m_imageRegion[1],
                  this->m_imageRegion[2],
                  this->m_imageRegion[3]);
  }

  virtual viskores::rendering::Camera camera(const viskores::Bounds &bounds) const = 0;

 protected:
  viskores::Vec3f_32 m_position;
  viskores::Vec3f_32 m_direction;
  viskores::Vec3f_32 m_up;
  viskores::Vec4f_32 m_imageRegion;
};

struct UnknownCamera : public Camera
{
  UnknownCamera(VTKmDeviceGlobalState *s);

  viskores::rendering::Camera camera(const viskores::Bounds &) const override;

  bool isValid() const override;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Camera *, ANARI_CAMERA);
