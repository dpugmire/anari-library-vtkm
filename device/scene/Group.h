// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "array/ObjectArray.h"
#include "light/Light.h"
#include "surface/Surface.h"
#include "volume/Volume.h"

namespace vtkm_device {

struct Group : public Object
{
  Group(VTKmDeviceGlobalState *s);
  ~Group() override;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  void commit() override;

  const std::vector<Surface *> &surfaces() const;
  const std::vector<Volume *> &volumes() const;

 private:
  void cleanup();

  // Geometry //

  helium::IntrusivePtr<ObjectArray> m_surfaceData;
  std::vector<Surface *> m_surfaces;

  // Volume //

  helium::IntrusivePtr<ObjectArray> m_volumeData;
  std::vector<Volume *> m_volumes;
};

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Group *, ANARI_GROUP);
