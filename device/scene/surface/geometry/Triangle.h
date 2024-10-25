// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
#include "array/Array1D.h"
#include "array/Array3D.h"
//helium
#include <helium/utility/ChangeObserverPtr.h>

#include <vtkm/rendering/Actor.h>

namespace vtkm_device {

struct Triangle : Geometry
{
  Triangle(VTKmDeviceGlobalState *s);

  void commit() override;
  virtual vtkm::rendering::Actor *actor() const override {return this->m_actor.get();}
  virtual vtkm::rendering::MapperRayTracer *mapper() const override {return this->m_mapper.get();}

 private:
  helium::ChangeObserverPtr<Array1D> m_index;
  helium::ChangeObserverPtr<Array1D> m_vertexPosition;
  // TODO: Add other attributes to observe changes.

  std::shared_ptr<vtkm::rendering::Actor> m_actor;
  std::shared_ptr<vtkm::rendering::MapperRayTracer> m_mapper;

  vtkm::cont::ColorTable m_colorTable;
  helium::ChangeObserverPtr<Array3D> m_dataArray;
};

} // namespace vtkm_device
