// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

#include <viskores/Matrix.h>
#include <viskores/rendering/Actor.h>

namespace viskores_device {

struct Geometry;

struct Sampler : public Object
{
  Sampler(ViskoresDeviceGlobalState *d);
  virtual ~Sampler();
  static Sampler *createInstance(
      std::string_view subtype, ViskoresDeviceGlobalState *d);

  void commitParameters() override;
  void finalize() override;

  const Mat4f_32& outTransform() const { return this->m_outTransform; }
  const viskores::Vec4f_32& outOffset() const { return this->m_outOffset; }

  virtual std::shared_ptr<viskores::rendering::Actor> createActor(const viskores::cont::DataSet& data) = 0;

 private:
  Mat4f_32 m_outTransform;
  viskores::Vec4f_32 m_outOffset;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(
    viskores_device::Sampler *, ANARI_SAMPLER);
