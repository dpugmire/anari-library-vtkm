// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Group.h"

namespace vtkm_device {

struct Instance : public Object
{
  Instance(VTKmDeviceGlobalState *s);
  ~Instance() override;

  void commit() override;

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

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_SPECIALIZATION(vtkm_device::Instance *, ANARI_INSTANCE);
