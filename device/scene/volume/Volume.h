// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace vtkm_device {

struct Volume : public Object
{
  Volume(VTKmDeviceGlobalState *d);
  virtual ~Volume();
  static Volume *createInstance(std::string_view subtype, VTKmDeviceGlobalState *d);

  void commit() override;

  uint32_t id() const;

  private:
  uint32_t m_id{~0u};
};

// Inlined definitions ////////////////////////////////////////////////////////

inline uint32_t Volume::id() const
{
  return m_id;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Volume *, ANARI_VOLUME);
