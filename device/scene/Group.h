// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "array/ObjectArray.h"
#include "light/Light.h"
#include "surface/Surface.h"
#include "volume/Volume.h"

namespace viskores_device {

struct Group : public Object
{
  Group(ViskoresDeviceGlobalState *s);
  ~Group() override;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  void commitParameters() override;
  void finalize() override;

  const std::vector<Surface *> &surfaces() const;
  const std::vector<Volume *> &volumes() const;

 private:
  helium::ChangeObserverPtr<ObjectArray> m_surfaceData;
  std::vector<Surface *> m_surfaces;

  helium::ChangeObserverPtr<ObjectArray> m_volumeData;
  std::vector<Volume *> m_volumes;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Group *, ANARI_GROUP);
