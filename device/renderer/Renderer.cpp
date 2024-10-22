// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "../VTKmTypes.h"
#include "Renderer.h"

namespace vtkm_device {

Renderer::Renderer(VTKmDeviceGlobalState *s) : Object(ANARI_RENDERER, s)
{
  s->objectCounts.renderers++;
}

Renderer::~Renderer()
{
  deviceState()->objectCounts.renderers--;
}

Renderer *Renderer::createInstance(
    std::string_view /* subtype */, VTKmDeviceGlobalState *s)
{
  return new Renderer(s);
}

void Renderer::commit()
{
  std::cout<<"Renderer::commit: "<<std::endl;
  //m_bgColor = getParam<float4>("background", float4(float3(0.f), 1.f));
  m_bgColor = getParam<float4>("background", float4(0.f, 0.f, 0.f, 1.f));
//  m_bgColor = float4(1.f, 0.f, 0.f, 1.f);
}

float4 Renderer::background() const
{
  return m_bgColor;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Renderer *);
