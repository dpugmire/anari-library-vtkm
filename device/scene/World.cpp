// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "World.h"
#include "surface/Surface.h"
#include "volume/Volume.h"

namespace viskores_device {

World::World(VTKmDeviceGlobalState *s)
    : Object(ANARI_WORLD, s),
      m_zeroSurfaceData(this),
      m_zeroVolumeData(this),
      m_instanceData(this)
{
  m_zeroGroup = new Group(s);
  m_zeroInstance = new Instance(s);
  m_zeroInstance->setParamDirect("group", m_zeroGroup.ptr);

  // never any public ref to these objects
  m_zeroGroup->refDec(helium::RefType::PUBLIC);
  m_zeroInstance->refDec(helium::RefType::PUBLIC);
}

World::~World() = default;

bool World::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  if (name == "bounds" && type == ANARI_FLOAT32_BOX3) {
    viskores::Vec3f_32 anariBounds[] = {viskores::Vec3f_32(this->m_bounds.MinCorner()),
        viskores::Vec3f_32(this->m_bounds.MaxCorner())};
    std::memcpy(ptr, &anariBounds, sizeof(anariBounds));
    return true;
  }
  return Object::getProperty(name, type, ptr, flags);
}

void World::commitParameters()
{
  m_zeroSurfaceData = getParamObject<ObjectArray>("surface");
  m_zeroVolumeData = getParamObject<ObjectArray>("volume");

  m_addZeroInstance = m_zeroSurfaceData || m_zeroVolumeData;
  if (m_addZeroInstance) {
    reportMessage(
        ANARI_SEVERITY_DEBUG, "viskores_device::World will add zero instance");
  }

  if (m_zeroSurfaceData) {
    reportMessage(ANARI_SEVERITY_DEBUG,
        "viskores_device::World found %zu surfaces in zero instance",
        m_zeroSurfaceData->size());
    m_zeroGroup->setParamDirect("surface", getParamDirect("surface"));
  } else {
    m_zeroGroup->removeParam("surface");
  }

  if (m_zeroVolumeData) {
    reportMessage(ANARI_SEVERITY_DEBUG,
        "viskores_device::World found %zu volumes in zero instance",
        m_zeroVolumeData->size());
    m_zeroGroup->setParamDirect("volume", getParamDirect("volume"));
  } else
    m_zeroGroup->removeParam("volume");

  m_zeroInstance->setParam("id", getParam<uint32_t>("id", ~0u));

  m_zeroGroup->commitParameters();
  m_zeroInstance->commitParameters();

  m_instanceData = getParamObject<ObjectArray>("instance");
}

void World::finalize()
{
  m_instances.clear();

  if (m_instanceData) {
    m_instanceData->removeAppendedHandles();
    if (m_addZeroInstance)
      m_instanceData->appendHandle(m_zeroInstance.ptr);
    std::for_each(m_instanceData->handlesBegin(),
        m_instanceData->handlesEnd(),
        [&](auto *o) {
          if (o && o->isValid())
            m_instances.push_back((Instance *)o);
        });
  } else if (m_addZeroInstance)
    m_instances.push_back(m_zeroInstance.ptr);

  if (m_instanceData)
    m_instanceData->addChangeObserver(this);
  if (m_zeroSurfaceData)
    m_zeroSurfaceData->addChangeObserver(this);

  this->m_bounds = viskores::Bounds{};
  for (auto &&instance : this->instances()) {
    for (auto &&surface : instance->group()->surfaces()) {
      this->m_bounds.Include(surface->bounds());
    }
    for (auto &&volume : instance->group()->volumes()) {
      this->m_bounds.Include(volume->bounds());
    }
  }
}

const std::vector<Instance *> &World::instances() const
{
  return m_instances;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::World *);
