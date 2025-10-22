// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Material.h"

namespace viskores_device {

Material::Material(ViskoresDeviceGlobalState *s) : Object(ANARI_MATERIAL, s) {}

Material::~Material() = default;

void Material::commitParameters()
{
  // no-op
}

void Material::finalize()
{
  // no-op
}

Material *Material::createInstance(
    std::string_view subtype, ViskoresDeviceGlobalState *s)
{
#if 0
  return (Material *)new UnknownObject(ANARI_MATERIAL, s);
#else
  (void)subtype;
  return new Material(s);
#endif
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Material *);
