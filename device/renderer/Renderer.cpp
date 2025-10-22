// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.h"
#include "../VTKmTypes.h"

namespace viskores_device {

Renderer::Renderer(VTKmDeviceGlobalState *s) : Object(ANARI_RENDERER, s) {}

Renderer::~Renderer() = default;

Renderer *Renderer::createInstance(
    std::string_view /* subtype */, VTKmDeviceGlobalState *s)
{
  return new Renderer(s);
}

void Renderer::commitParameters()
{
  m_bgColor = getParam<float4>("background", float4(0.f, 0.f, 0.f, 1.f));
}

void Renderer::finalize()
{
  // no-op
}

float4 Renderer::background() const
{
  return m_bgColor;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Renderer *);
