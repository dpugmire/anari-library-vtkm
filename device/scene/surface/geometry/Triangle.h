// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
#include "array/Array1D.h"
// viskores
#include <viskores/rendering/Actor.h>

namespace viskores_device {

struct Triangle : Geometry
{
  Triangle(VTKmDeviceGlobalState *s);

  void commitParameters() override;
  void finalize() override;

  virtual viskores::rendering::Actor *actor() const override {return this->m_actor.get();}
  virtual viskores::rendering::MapperRayTracer *mapper() const override {return this->m_mapper.get();}

  bool isValid() const override;

 private:
  helium::ChangeObserverPtr<Array1D> m_index;
  helium::ChangeObserverPtr<Array1D> m_vertexPosition;
  // TODO: Add other attributes to observe changes.

  std::shared_ptr<viskores::rendering::Actor> m_actor;
  std::shared_ptr<viskores::rendering::MapperRayTracer> m_mapper;

  viskores::cont::ColorTable m_colorTable;
  helium::ChangeObserverPtr<Array1D> m_vertexColor;
};

} // namespace viskores_device
