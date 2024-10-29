// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Volume.h"
#include "array/Array1D.h"
#include "spatial_field/SpatialField.h"
// helium
#include <helium/utility/ChangeObserverPtr.h>
// VTK-m
#include <vtkm/Range.h>
#include <vtkm/cont/ColorTable.h>
#include <vtkm/rendering/MapperVolume.h>

namespace vtkm_device
{

struct TransferFunction1D : public Volume
{
  TransferFunction1D(VTKmDeviceGlobalState *d);

  void commit() override;

  const SpatialField *spatialField() const;
  vtkm::Bounds bounds() const override;
  vtkm::rendering::Actor *actor() const override;
  vtkm::rendering::MapperVolume *mapper() const override;

  bool isValid() const override;

 private:
  helium::ChangeObserverPtr<SpatialField> m_spatialField;
  vtkm::Range m_valueRange;
  helium::ChangeObserverPtr<Array1D> m_colorArray;
  helium::ChangeObserverPtr<Array1D> m_opacity;
  vtkm::Float32 m_unitDistance;
  vtkm::cont::ColorTable m_colorTable;

  std::shared_ptr<vtkm::rendering::Actor> m_actor;
  std::shared_ptr<vtkm::rendering::MapperVolume> m_mapper;
};

} // namespace vtkm_device
