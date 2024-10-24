// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
#include "VTKmTypes.h"
// VTK-m
#include <vtkm/Bounds.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/Mapper.h>

namespace vtkm_device {

struct Volume : public Object
{
  Volume(VTKmDeviceGlobalState *d);
  virtual ~Volume();
  static Volume *createInstance(
      std::string_view subtype, VTKmDeviceGlobalState *d);

  void commit() override;

  uint32_t id() const;

  virtual vtkm::Bounds bounds() const = 0;
  virtual vtkm::rendering::Actor *actor() const = 0;
  virtual vtkm::rendering::Mapper *mapper() const = 0;

  bool isValid() const override;

 private:
  uint32_t m_id{~0u};
};

struct UnknownVolume : public Volume
{
  UnknownVolume(VTKmDeviceGlobalState *d);

  vtkm::Bounds bounds() const override;
  vtkm::rendering::Actor *actor() const override;
  vtkm::rendering::Mapper *mapper() const override;
  bool isValid() const override;
};

// Inlined definitions ////////////////////////////////////////////////////////

inline uint32_t Volume::id() const
{
  return m_id;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Volume *, ANARI_VOLUME);
