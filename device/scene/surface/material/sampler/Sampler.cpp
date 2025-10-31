// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Sampler.h"
#include "Image1DSampler.h"
namespace {

struct UnknownSampler : viskores_device::Sampler
{
  UnknownSampler(viskores_device::ViskoresDeviceGlobalState *d);

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

  std::shared_ptr<viskores::rendering::Actor> createActor(
      const viskores::cont::DataSet &data) override;
};

UnknownSampler::UnknownSampler(viskores_device::ViskoresDeviceGlobalState *d)
    : Sampler(d)
{}

void UnknownSampler::commitParameters()
{
  // invalid
}
void UnknownSampler::finalize()
{
  // invalid
}

bool UnknownSampler::isValid() const
{
  return false;
}

std::shared_ptr<viskores::rendering::Actor> UnknownSampler::createActor(
    const viskores::cont::DataSet &)
{
  return nullptr;
}

} // anonymous namespace

namespace viskores_device {

Sampler::Sampler(ViskoresDeviceGlobalState *s) : Object(ANARI_SAMPLER, s) {}

Sampler::~Sampler() = default;

Sampler *Sampler::createInstance(
    std::string_view subtype, ViskoresDeviceGlobalState *s)
{
  if (subtype == "image1D") {
    return new Image1DSampler(s);
  } else {
    return new UnknownSampler(s);
  }
}

void Sampler::commitParameters()
{
  mat4 outTransform = this->getParam("outTransform", mat4(linalg::identity));
  this->m_outTransform = toViskoresMatrix(outTransform);
  float4 outOffset = this->getParam("outOffset", float4(0.f, 0.f, 0.f, 0.f));
  this->m_outOffset = { outOffset[0], outOffset[1], outOffset[2], outOffset[3] };
}

void Sampler::finalize()
{
  // no-op
}

} // namespace viskores_device

VISKORES_ANARI_TYPEFOR_DEFINITION(viskores_device::Sampler *);
