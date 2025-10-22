// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ViskoresDeviceGlobalState.h"
#include "viskores_device_math.h"
// helium
#include <helium/BaseObject.h>
#include <helium/utility/ChangeObserverPtr.h>
// std
#include <string_view>

namespace viskores_device {

struct Object : public helium::BaseObject
{
  Object(ANARIDataType type, ViskoresDeviceGlobalState *s);
  virtual ~Object() = default;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  bool isValid() const override;

  ViskoresDeviceGlobalState *deviceState() const;

  void printParameters();
};

struct UnknownObject : public Object
{
  UnknownObject(ANARIDataType type, ViskoresDeviceGlobalState *s);
  ~UnknownObject() override;
  void commitParameters() override;
  void finalize() override;
  bool isValid() const override;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Object *, ANARI_OBJECT);
