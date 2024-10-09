// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Group.h"
// std
#include <iterator>

namespace vtkm_device {

Group::Group(VTKmDeviceGlobalState *s) : Object(ANARI_GROUP, s)
{
  s->objectCounts.groups++;
}

Group::~Group()
{
  cleanup();
  deviceState()->objectCounts.groups--;
}

bool Group::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  return Object::getProperty(name, type, ptr, flags);
}

void Group::commit()
{
  cleanup();

  m_surfaceData = getParamObject<ObjectArray>("surface");
  m_volumeData = getParamObject<ObjectArray>("volume");

  if (m_surfaceData) {
    m_surfaceData->addChangeObserver(this);
    std::transform(m_surfaceData->handlesBegin(),
        m_surfaceData->handlesEnd(),
        std::back_inserter(m_surfaces),
        [](auto *o) { return (Surface *)o; });
  }

  if (m_volumeData) {
    m_volumeData->addChangeObserver(this);
    std::transform(m_volumeData->handlesBegin(),
        m_volumeData->handlesEnd(),
        std::back_inserter(m_volumes),
        [](auto *o) { return (Volume *)o; });
  }
}

const std::vector<Surface *> &Group::surfaces() const
{
  std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
  return m_surfaces;
}

const std::vector<Volume *> &Group::volumes() const
{
  std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
  return m_volumes;
}

static inline float clampIt(const float &a, const range_t<float> &r)
{
  if (a < r.lower) return r.lower;
  if (a > r.upper) return r.upper;
  return a;
}

static inline float CLAMP(const float& a, const box1& b)
{
  if (a < b.lower)
    return b.lower;
  else if (a > b.upper)
    return b.upper;

  return a;
}

static inline float3 MIN(const float3& a, const float3& b)
{
  float3 res(std::min(a[0], b[0]),
             std::min(a[1], b[1]),
             std::min(a[2], b[2]));
  return res;
}
static inline float3 MAX(const float3& a, const float3& b)
{
  float3 res(std::max(a[0], b[0]),
             std::max(a[1], b[1]),
             std::max(a[2], b[2]));
  return res;
}

static inline float MINELEM(const float3& a)
{
  return std::min(a[0], std::min(a[1],a[2]));
}
static inline float MAXELEM(const float3& a)
{
  return std::max(a[0], std::max(a[1],a[2]));
}

//static inline float MAX(const float& a, const float& b)
//{
//  return a > b ? a : b;
//}

void Group::intersectVolumes(VolumeRay &ray) const
{
  Volume *originalVolume = ray.volume;
  box1 t = ray.t;

  for (auto *v : volumes())
  {
    if (!v->isValid())
      continue;

    const box3 bounds = v->bounds();
    const float3 mins = (bounds.lower - ray.org) * (1.f / ray.dir);
    const float3 maxs = (bounds.upper - ray.org) * (1.f / ray.dir);

    const float3 nears = MIN(mins, maxs);
    const float3 fars = MAX(mins, maxs);

    const box1 lt(MAXELEM(nears), MINELEM(fars));

    //Check for hit..
    if (lt.lower < lt.upper && (!ray.volume || lt.lower < t.lower))
    {
      t.lower = CLAMP(lt.lower, t);
      t.upper = CLAMP(lt.upper, t);
      ray.volume = v;
    }
  }

  if (ray.volume != originalVolume)
    ray.t = t;
}


void Group::cleanup()
{
  if (m_surfaceData)
    m_surfaceData->removeChangeObserver(this);
  if (m_volumeData)
    m_volumeData->removeChangeObserver(this);

  m_surfaces.clear();
  m_volumes.clear();
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Group *);
