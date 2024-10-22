// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Object.h"

namespace vtkm_device {

// Object definitions /////////////////////////////////////////////////////////

Object::Object(ANARIDataType type, VTKmDeviceGlobalState *s)
    : helium::BaseObject(type, s)
{}

void Object::commit()
{
  // no-op
}

bool Object::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
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

bool UnknownObject::isValid() const
{
  return false;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Object *);
