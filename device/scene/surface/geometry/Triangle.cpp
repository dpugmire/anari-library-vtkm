// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Triangle.h"
// VTK-m
#include <vtkm/CellShape.h>
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/ArrayHandleRuntimeVec.h>
#include <vtkm/cont/CellSetSingleType.h>
#include <vtkm/cont/UnknownArrayHandle.h>

namespace vtkm_device {

Triangle::Triangle(VTKmDeviceGlobalState* s)
    : Geometry(s), m_index(this), m_vertexPosition(this), m_dataArray(this)
{
}

void Triangle::commit()
{
  // Stashing these in a ChangeObserverPtr means that commit will be
  // called again if the array contents change.
  this->m_index = getParamObject<Array1D>("primitive.index");
  this->m_vertexPosition = getParamObject<Array1D>("vertex.position");

  // Reset data
  this->m_dataSet = vtkm::cont::DataSet{};
  this->m_mapper = std::make_shared<vtkm::rendering::MapperRayTracer>();

  // Get the connection array.
  // Note that ANARI provides the connection array as a series of triples whereas
  // VTK-m wants a flat array of indices. The easist way to do the conversion
  // (while sharing pointers) is to use ArrayHandleRuntimeVec.
  vtkm::cont::ArrayHandleRuntimeVec<vtkm::Id> connectionArray(3);
  vtkm::cont::ArrayCopyShallowIfPossible(this->m_index->dataAsVTKmArray(), connectionArray);

  vtkm::cont::CellSetSingleType<> cellSet;
  cellSet.Fill(
      static_cast<vtkm::Id>(this->m_vertexPosition->size()),
      vtkm::CELL_SHAPE_TRIANGLE,
      3,
      connectionArray.GetComponentsArray());
  this->m_dataSet.SetCellSet(cellSet);

  this->m_dataSet.AddCoordinateSystem({ "coords", this->m_vertexPosition->dataAsVTKmArray() });

  this->m_dataArray = getParamObject<Array3D>("vertex.color");
  vtkm::cont::UnknownArrayHandle vtkmArray = this->m_dataArray->dataAsVTKmArray();
  if (!vtkmArray.IsValueType<vtkm::Float32>() && !vtkmArray.IsValueType<vtkm::Float64>())
  {
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> castArray;
    vtkm::cont::ArrayCopy(vtkmArray, castArray);
    vtkmArray = castArray;
  }
  this->m_dataSet.AddPointField("data", vtkmArray);

  box1 range = { 0, 1 };
  this->getParam("valueRange", ANARI_FLOAT32_BOX1, &range);
  this->m_colorTable.RescaleToRange({ range.lower, range.upper });


  this->m_actor = std::make_shared<vtkm::rendering::Actor>(
      this->m_dataSet.GetCellSet(),
      this->m_dataSet.GetCoordinateSystem(),
      this->m_dataSet.GetField("data"),
      this->m_colorTable);

  this->AddAttributeInformation();
}

} // namespace vtkm_device
