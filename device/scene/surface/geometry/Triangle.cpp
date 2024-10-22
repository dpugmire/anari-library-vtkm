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
    : Geometry(s), m_index(this), m_vertexPosition(this)
{
}

void Triangle::commit()
{
  // Stashing these in a ChangeObserverPtr means that commit will be
  // called again if the array contents change.
  this->m_index = getParamObject<Array1D>("primitive.index");
  this->m_vertexPosition = getParamObject<Array1D>("vertex.position");

  // Reset data
  this->m_data = vtkm::cont::DataSet{};

  // Get the connection array.
  // Note that ANARI provides the connection array as a series of triples whereas
  // VTK-m wants a flat array of indices. The easist way to do the conversion
  // (while sharing pointers) is to use ArrayHandleRuntimeVec.
  vtkm::cont::ArrayHandleRuntimeVec<vtkm::Id> connectionArray(3);
  vtkm::cont::ArrayCopyShallowIfPossible(this->m_index->dataAsVTKmArray(), connectionArray);

  vtkm::cont::CellSetSingleType<> cellSet;
  cellSet.Fill(connectionArray.GetNumberOfValues() / 3,
      vtkm::CELL_SHAPE_TRIANGLE,
      3,
      connectionArray.GetComponentsArray());
  this->m_data.SetCellSet(cellSet);

  this->m_data.AddCoordinateSystem({ "coords", this->m_vertexPosition->dataAsVTKmArray() });

  this->AddAttributeInformation();

  // this->m_data.PrintSummary(std::cout);
}

} // namespace vtkm_device
