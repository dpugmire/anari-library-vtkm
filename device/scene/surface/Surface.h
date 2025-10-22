// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "geometry/Geometry.h"
#include "material/Material.h"
// VTK-m
#include <vtkm/Bounds.h>

namespace vtkm_device {

struct Surface : public Object
{
  Surface(VTKmDeviceGlobalState *s);
  ~Surface() override;

  void commitParameters() override;
  void finalize() override;

  uint32_t id() const {return m_id;}
  const Geometry *geometry() const;
  const Material *material() const;

  vtkm::Bounds bounds() const;

  bool isValid() const override;

 private:
  uint32_t m_id{~0u};
  helium::IntrusivePtr<Geometry> m_geometry;
  helium::IntrusivePtr<Material> m_material;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Surface *, ANARI_SURFACE);
