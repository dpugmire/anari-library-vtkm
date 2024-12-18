// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
// VTK-m
#include <vtkm/Bounds.h>

namespace vtkm_device {

struct World : public Object
{
  World(VTKmDeviceGlobalState *s);
  ~World() override;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  void commit() override;

  const std::vector<Instance *> &instances() const;

  const Instance *instanceFromRay(const Ray &ray) const { return this->instances()[ray.instID]; }
  const Instance *instanceFromRay(const VolumeRay &ray) const { return this->instances()[ray.instID]; }
  const Surface *surfaceFromRay(const Ray &ray) const { return instanceFromRay(ray)->group()->surfaces()[ray.geomID]; }

  const vtkm::Bounds &bounds() const { return this-> m_bounds; }

 private:
  helium::ChangeObserverPtr<ObjectArray> m_zeroSurfaceData;
  helium::ChangeObserverPtr<ObjectArray> m_zeroVolumeData;

  helium::ChangeObserverPtr<ObjectArray> m_instanceData;
  std::vector<Instance *> m_instances;

  bool m_addZeroInstance{false};
  helium::IntrusivePtr<Group> m_zeroGroup;
  helium::IntrusivePtr<Instance> m_zeroInstance;

  vtkm::Bounds m_bounds;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::World *, ANARI_WORLD);
