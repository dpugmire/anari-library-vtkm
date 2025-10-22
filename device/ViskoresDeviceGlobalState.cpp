// Copyright 2023 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "ViskoresDeviceGlobalState.h"
#include "frame/Frame.h"

namespace viskores_device {

ViskoresDeviceGlobalState::ViskoresDeviceGlobalState(ANARIDevice d)
    : helium::BaseGlobalDeviceState(d)
{}

void ViskoresDeviceGlobalState::waitOnCurrentFrame() const
{
  if (currentFrame)
    currentFrame->wait();
}

} // namespace viskores_device