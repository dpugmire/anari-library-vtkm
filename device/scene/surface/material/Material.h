// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
#include "sampler/Sampler.h"

#include <viskores/rendering/Actor.h>

namespace viskores_device {

struct Material : public Object
{
  Material(ViskoresDeviceGlobalState *s);
  ~Material() override;
  static Material *createInstance(
      std::string_view subtype, ViskoresDeviceGlobalState *s);

  virtual std::shared_ptr<viskores::rendering::Actor> createActor(const viskores::cont::DataSet& data) = 0;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Material *, ANARI_MATERIAL);
