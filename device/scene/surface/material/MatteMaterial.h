#pragma once

#include "Material.h"
#include "sampler/Sampler.h"
// helium
#include <helium/utility/ChangeObserverPtr.h>

namespace viskores_device {

struct MatteMaterial : public Material
{
  MatteMaterial(ViskoresDeviceGlobalState *d);

  void commitParameters() override;
  void finalize() override;

  std::shared_ptr<viskores::rendering::Actor> createActor(
      const viskores::cont::DataSet &data) override;

  Sampler *sampler() const
  {
    return this->m_sampler.get();
  }

  helium::Attribute colorAttribute() const
  {
    return this->m_colorAttribute;
  }
  const viskores::Vec3f_32 &color() const
  {
    return this->m_color;
  }

  helium::Attribute opacityAttribute() const
  {
    return this->m_opacityAttribute;
  }
  viskores::Float32 opacity() const
  {
    return this->m_opacity;
  }

  helium::AlphaMode alphaMode() const
  {
    return this->m_alphaMode;
  }
  viskores::Float32 alphaCutoff() const
  {
    return this->m_alphaCutoff;
  }

 private:
  helium::ChangeObserverPtr<Sampler> m_sampler;
  helium::Attribute m_colorAttribute;
  viskores::Vec3f_32 m_color;

  helium::Attribute m_opacityAttribute;
  viskores::Float32 m_opacity;

  helium::AlphaMode m_alphaMode;
  viskores::Float32 m_alphaCutoff;
};

} // namespace viskores_device
