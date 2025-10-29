// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Material.h"

#include "MatteMaterial.h"

namespace {

struct UnknownMaterial : viskores_device::Material
{
  UnknownMaterial(viskores_device::ViskoresDeviceGlobalState *d);

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;
};

UnknownMaterial::UnknownMaterial(viskores_device::ViskoresDeviceGlobalState *d)
    : Material(d)
{}

void UnknownMaterial::commitParameters()
{
  // invalid
}
void UnknownMaterial::finalize()
{
  // invalid
}

bool UnknownMaterial::isValid() const
{
  return false;
}

} // anonymous namespace

namespace viskores_device {

Material::Material(ViskoresDeviceGlobalState *s) : Object(ANARI_MATERIAL, s) {}

Material::~Material() = default;

Material *Material::createInstance(
    std::string_view subtype, ViskoresDeviceGlobalState *s)
{
  if (subtype == "matte") {
    return new MatteMaterial(s);
  } else {
    return new UnknownMaterial(s);
  }
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Material *);
