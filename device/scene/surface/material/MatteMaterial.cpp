#include "MatteMaterial.h"

namespace viskores_device {

MatteMaterial::MatteMaterial(ViskoresDeviceGlobalState *d) : Material(d) {}

void MatteMaterial::commitParameters()
{
  this->m_colorAttribute =
      helium::attributeFromString(this->getParamString("color", "none"));
  this->m_color = this->getParam("color", viskores::Vec3f_32{0.8, 0.8, 0.8});
  // TODO: Implement sampler

  this->m_opacityAttribute =
      helium::attributeFromString(this->getParamString("color", "none"));
  this->m_opacity = this->getParam("opacity", viskores::Float32{1.0f});
  // TODO: Implement sampler

  this->m_alphaMode =
      helium::alphaModeFromString(this->getParamString("alphaMode", "opaque"));

  this->m_alphaCutoff = this->getParam("alphaCutoff", viskores::Float32{0.5f});
}

void MatteMaterial::finalize()
{
  // no-op
}

} // namespace viskores_device
