// Copyright 2023 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "VTKmDeviceGlobalState.h"
#include "frame/Frame.h"

namespace vtkm_device {

VTKmDeviceGlobalState::VTKmDeviceGlobalState(ANARIDevice d)
    : helium::BaseGlobalDeviceState(d)
{}

void VTKmDeviceGlobalState::waitOnCurrentFrame() const
{
  if (currentFrame)
    currentFrame->wait();
}

} // namespace vtkm_device