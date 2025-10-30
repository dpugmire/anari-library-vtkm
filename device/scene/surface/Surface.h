// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "geometry/Geometry.h"
#include "material/Material.h"
// Viskores
#include <viskores/Bounds.h>

namespace viskores_device {

struct Surface : public Object
{
  Surface(ViskoresDeviceGlobalState *s);
  ~Surface() override;

  void commitParameters() override;
  void finalize() override;

  uint32_t id() const
  {
    return m_id;
  }
  const Geometry *geometry() const;
  const Material *material() const;

  virtual viskores::rendering::Actor *actor() const
  {
    return this->m_actor.get();
  }

  viskores::Bounds bounds() const;

  bool isValid() const override;

 private:
  uint32_t m_id{~0u};
  helium::ChangeObserverPtr<Geometry> m_geometry;
  helium::ChangeObserverPtr<Material> m_material;

  std::shared_ptr<viskores::rendering::Actor> m_actor;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(
    viskores_device::Surface *, ANARI_SURFACE);
