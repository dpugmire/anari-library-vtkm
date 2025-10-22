// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Object.h"

namespace viskores_device {

// Object definitions /////////////////////////////////////////////////////////

Object::Object(ANARIDataType type, VTKmDeviceGlobalState *s)
    : helium::BaseObject(type, s)
{}

bool Object::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  (void)flags;  // ignored
  if (name == "valid" && type == ANARI_BOOL) {
    helium::writeToVoidP(ptr, isValid());
    return true;
  }

  return false;
}

bool Object::isValid() const
{
  return true;
}

VTKmDeviceGlobalState *Object::deviceState() const
{
  return (VTKmDeviceGlobalState *)helium::BaseObject::m_state;
}

void Object::printParameters()
{
  for (auto&& paramItr = this->params_begin(); paramItr != this->params_end(); ++paramItr)
  {
    std::cout << paramItr->first << "\n";
  }
}

// UnknownObject definitions //////////////////////////////////////////////////

UnknownObject::UnknownObject(ANARIDataType type, VTKmDeviceGlobalState *s)
    : Object(type, s)
{
  s->objectCounts.unknown++;
}

UnknownObject::~UnknownObject()
{
  deviceState()->objectCounts.unknown--;
}

void UnknownObject::commitParameters()
{
  // no-op
}

void UnknownObject::finalize()
{
  // no-op
}

bool UnknownObject::isValid() const
{
  return false;
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Object *);
