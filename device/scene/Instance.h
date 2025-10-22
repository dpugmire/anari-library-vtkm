// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Group.h"

namespace viskores_device {

struct Instance : public Object
{
  Instance(VTKmDeviceGlobalState *s);
  ~Instance() override;

  void commitParameters() override;
  void finalize() override;

  uint32_t id() const;

  const mat4 &xfm() const;
  const mat3 &xfmInvRot() const;
  bool xfmIsIdentity() const;

  const Group *group() const;
  Group *group();

  bool isValid() const override;

 private:
  uint32_t m_id{~0u};
  mat4 m_xfm;
  mat3 m_xfmInvRot;
  helium::IntrusivePtr<Group> m_group;
};

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_SPECIALIZATION(viskores_device::Instance *, ANARI_INSTANCE);
