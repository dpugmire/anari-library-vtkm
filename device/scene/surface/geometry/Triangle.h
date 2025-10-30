// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
#include "array/Array1D.h"
// viskores
#include <viskores/rendering/Actor.h>
// std
#include <map>

namespace viskores_device {

struct Triangle : Geometry
{
  Triangle(ViskoresDeviceGlobalState *s);

  void commitParameters() override;
  void finalize() override;

  virtual viskores::rendering::Mapper *mapper() const override
  {
    return this->m_mapper.get();
  }

  bool isValid() const override;

 private:
  helium::ChangeObserverPtr<Array1D> m_index;
  std::map<std::string, helium::ChangeObserverPtr<Array1D>> m_vertexAttributes;
  std::map<std::string, helium::ChangeObserverPtr<Array1D>>
      m_faceVaryingAttributes;
  helium::ChangeObserverPtr<Array1D> &vertexAttribute(const std::string name)
  {
    return this->m_vertexAttributes.find(name)->second;
  }
  const helium::ChangeObserverPtr<Array1D> &vertexAttribute(
      const std::string name) const
  {
    return this->m_vertexAttributes.find(name)->second;
  }

  std::shared_ptr<viskores::rendering::MapperRayTracer> m_mapper;

  viskores::cont::ColorTable m_colorTable;
  helium::ChangeObserverPtr<Array1D> m_vertexColor;
};

} // namespace viskores_device
