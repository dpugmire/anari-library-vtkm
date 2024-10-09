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

PixelSample Renderer::renderSample(const float2 &screen, Ray ray, const World &w) const
{
  //Taken from helide Renderer.cpp

  PixelSample retval;

  const bool hitGeometry = false; //ray.geomID != RTC_INVALID_GEOMETRY_ID;

  VolumeRay vray;
  vray.org = ray.org;
  vray.dir = ray.dir;

  w.intersectVolumes(vray);
  const bool hitVolume = vray.volume != nullptr;

  retval.color = this->shadeRay(screen, ray, vray, w);
  retval.depth = hitVolume ? std::min(ray.tfar, vray.t.lower) : ray.tfar;
  if (hitGeometry || hitVolume)
  {
    retval.primId = hitVolume ? 0 : ray.primID;
    retval.objId = hitVolume ? vray.volume->id() : w.surfaceFromRay(ray)->id();
    retval.instId = hitVolume ? w.instanceFromRay(vray)->id()
                              : w.instanceFromRay(ray)->id();
  }

  return retval;
}

float4 Renderer::shadeRay(const float2 &screen,
    const Ray &ray,
    const VolumeRay &vray,
    const World &w) const
{
  const bool hitGeometry = false; //ray.geomID != RTC_INVALID_GEOMETRY_ID;
  const bool hitVolume = vray.volume != nullptr;

  const float4 bgColorOpacity = this->m_bgColor;
//      m_bgImage ? backgroundColorFromImage(*m_bgImage, screen) : m_bgColor;

  if (!hitGeometry && !hitVolume)
    return bgColorOpacity;

  const float3 bgColor(bgColorOpacity[0], bgColorOpacity[1], bgColorOpacity[2]);

  float3 color(0.f, 0.f, 0.f);
  float opacity = 0.f;

  float3 volumeColor = color;
  float volumeOpacity = 0.f;

  float3 geometryColor(0.f, 0.f, 0.f);
  float geometryOpacity = hitGeometry ? 1.f : 0.f;

  if (hitGeometry)
  {
  }

  if (hitVolume)
    vray.volume->render(vray, volumeColor, volumeOpacity);

  //color = linalg::min(volumeColor, float3(1.f));
  for (int i = 0; i < 3; i++) color[i] = std::min(volumeColor[i], 1.f);
  opacity = volumeOpacity;

/*
  accumulateValue(color, geometryColor * geometryOpacity, opacity);
  accumulateValue(opacity, geometryOpacity, opacity);
  accumulateValue(color, bgColor, opacity);
  accumulateValue(opacity, bgColorOpacity.w, opacity);
*/

  float4 val;//(color[0], color[1], color[2], opacity);
  for (int i = 0; i < 3; i++) val[i] = color[i];
  val[3] = opacity;

  return val;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Renderer *);
