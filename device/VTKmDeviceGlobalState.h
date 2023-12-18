// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderingSemaphore.h"
#include "vtkm_device_math.h"
// helium
#include "helium/BaseGlobalDeviceState.h"
// std
#include <atomic>
#include <mutex>

namespace vtkm_device {

struct Frame;

struct VTKmDeviceGlobalState : public helium::BaseGlobalDeviceState
{
  struct ObjectCounts
  {
    std::atomic<size_t> frames{0};
    std::atomic<size_t> cameras{0};
    std::atomic<size_t> renderers{0};
    std::atomic<size_t> worlds{0};
    std::atomic<size_t> instances{0};
    std::atomic<size_t> groups{0};
    std::atomic<size_t> surfaces{0};
    std::atomic<size_t> geometries{0};
    std::atomic<size_t> materials{0};
    std::atomic<size_t> samplers{0};
    std::atomic<size_t> volumes{0};
    std::atomic<size_t> spatialFields{0};
    std::atomic<size_t> arrays{0};
    std::atomic<size_t> unknown{0};
  } objectCounts;

  RenderingSemaphore renderingSemaphore;
  Frame *currentFrame{nullptr};

  // Helper methods //

  VTKmDeviceGlobalState(ANARIDevice d);
  void waitOnCurrentFrame() const;
};

// Helper functions/macros ////////////////////////////////////////////////////

inline VTKmDeviceGlobalState *asVTKmDeviceState(
    helium::BaseGlobalDeviceState *s)
{
  return (VTKmDeviceGlobalState *)s;
}

#define VTKM_ANARI_TYPEFOR_SPECIALIZATION(type, anari_type)                    \
  namespace anari {                                                            \
  ANARI_TYPEFOR_SPECIALIZATION(type, anari_type);                              \
  }

#define VTKM_ANARI_TYPEFOR_DEFINITION(type)                                    \
  namespace anari {                                                            \
  ANARI_TYPEFOR_DEFINITION(type);                                              \
  }

} // namespace vtkm_device
