// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Volume.h"
//#include "TransferFunction1D.h"
#include <iostream>
#include <vtkm/cont/DataSetBuilderUniform.h>

namespace vtkm_device {

Volume::Volume(VTKmDeviceGlobalState *s) : Object(ANARI_VOLUME, s)
{
  s->objectCounts.volumes++;
  this->MinCorner[0] = -1.f;
  this->MinCorner[1] = -1.f;
  this->MinCorner[2] = -1.f;
  this->MaxCorner[0] = 1.f;
  this->MaxCorner[1] = 1.f;
  this->MaxCorner[2] = 1.f;

  vtkm::Vec3f origin(0,0,0), spacing(.1, .1, .1);
  vtkm::cont::DataSetBuilderUniform dsb;
  this->DataSet = dsb.Create({10,10,10}, origin, spacing);
}

Volume::~Volume()
{
  deviceState()->objectCounts.volumes--;
}

Volume *Volume::createInstance(std::string_view subtype, VTKmDeviceGlobalState *s)
{
  std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
  return new Volume(s);

//  if (subtype =="transferFunction1D")
//    return new TransferFunction1D(s);

  return (Volume *)new UnknownObject(ANARI_VOLUME, s);
}

void Volume::commit()
{
  std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
  m_id = getParam<uint32_t>("id", ~0u);
}

box3 Volume::bounds() const
{
  auto dsBounds = this->DataSet.GetCoordinateSystem().GetBounds();
  float3 minC(dsBounds.X.Min, dsBounds.Y.Min, dsBounds.Z.Min);
  float3 maxC(dsBounds.X.Max, dsBounds.Y.Max, dsBounds.Z.Max);
  box3 bounds(minC, maxC);

  return bounds;
}
static bool intersectRayWithBoundingBox(const VolumeRay& ray, vtkm::Bounds box, float& tmin, float& tmax)
{
  tmin = 0.0f;
  tmax = std::numeric_limits<float>::max();

  float boxMin[3] = {(float)box.X.Min, (float)box.Y.Min, (float)box.Z.Min};
  float boxMax[3] = {(float)box.X.Max, (float)box.Y.Max, (float)box.Z.Max};
  for (int i = 0; i < 3; ++i)
  {
    float invD = 1.0 / ray.dir[i];
    float t0 = (boxMin[i] - ray.org[i]) * invD;
    float t1 = (boxMax[i] - ray.org[i]) * invD;

    if (invD < 0.0f) std::swap(t0, t1);

    tmin = std::max(tmin, t0);
    tmax = std::min(tmax, t1);

    if (tmax <= tmin)
      return false;
  }
    return true;
}

void Volume::render(const VolumeRay &vray,
                    float3 &color,
                    float &opacity)
{

  float tmin, tmax;
  bool val = intersectRayWithBoundingBox(vray, this->DataSet.GetCoordinateSystem().GetBounds(), tmin, tmax);

  color = float3(0,0,0);
  opacity = 0.0;
  if (val)
  {
    color = float3(1,0,0);
    opacity = 0.5;
    //std::cout<<"Hit!"<<std::endl;
  }
#if 0

  std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
  const float stepSize = 0.01f; //field()->stepSize();
  const float jitter = 1.f; // NOTE: use uniform rng if/when lower sampling rate
  box1 currentInterval = vray.t;
  currentInterval.lower += stepSize * jitter;

  while (opacity < 0.99f ) //&& size(currentInterval) >= 0.f)
  {
    const float3 p = vray.org + vray.dir * currentInterval.lower;
    //const float s0 = field()->sampleAt(p);
    const float s = 0.9293; //field()->sampleAt(p);

    if (!std::isnan(s))
    {
      //const float3 c = colorOf(s);
      const float3 c(1,1,0);
      const float o = .4;
      //const float o = opacityOf(s) * m_densityScale;
      accumulateValue(color, c * o, opacity);
      accumulateValue(opacity, o, opacity);
    }

    currentInterval.lower += stepSize;
  }
  #endif
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Volume *);
