// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Volume.h"
//#include "TransferFunction1D.h"
#include <iostream>

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
  box3 bounds(this->MinCorner, this->MaxCorner);

  return bounds;
}

void Volume::render(const VolumeRay &vray,
                    float3 &color,
                    float &opacity)
{
  std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
  const float stepSize = 0.01f; //field()->stepSize();
  const float jitter = 1.f; // NOTE: use uniform rng if/when lower sampling rate
  box1 currentInterval = vray.t;
  currentInterval.lower += stepSize * jitter;

  while (opacity < 0.99f ) //&& size(currentInterval) >= 0.f)
  {
    const float3 p = vray.org + vray.dir * currentInterval.lower;
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
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Volume *);
