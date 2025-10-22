// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
// Viskores
#include <viskores/Bounds.h>

namespace viskores_device {

struct World : public Object
{
  World(VTKmDeviceGlobalState *s);
  ~World() override;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  void commitParameters() override;
  void finalize() override;

  const std::vector<Instance *> &instances() const;

  const Instance *instanceFromRay(const Ray &ray) const { return this->instances()[ray.instID]; }
  const Instance *instanceFromRay(const VolumeRay &ray) const { return this->instances()[ray.instID]; }
  const Surface *surfaceFromRay(const Ray &ray) const { return instanceFromRay(ray)->group()->surfaces()[ray.geomID]; }

  const viskores::Bounds &bounds() const { return this-> m_bounds; }

 private:
  helium::ChangeObserverPtr<ObjectArray> m_zeroSurfaceData;
  helium::ChangeObserverPtr<ObjectArray> m_zeroVolumeData;

  helium::ChangeObserverPtr<ObjectArray> m_instanceData;
  std::vector<Instance *> m_instances;

  bool m_addZeroInstance{false};
  helium::IntrusivePtr<Group> m_zeroGroup;
  helium::IntrusivePtr<Instance> m_zeroInstance;

  viskores::Bounds m_bounds;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::World *, ANARI_WORLD);
