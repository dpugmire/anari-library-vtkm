// Copyright 2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Triangle.h"
// Viskores
#include <viskores/CellShape.h>
#include <viskores/cont/ArrayCopy.h>
#include <viskores/cont/ArrayHandleRuntimeVec.h>
#include <viskores/cont/CellSetSingleType.h>
#include <viskores/cont/UnknownArrayHandle.h>
// std
#include <array>
#include <numeric>

namespace viskores_device {

Triangle::Triangle(ViskoresDeviceGlobalState *s)
    : Geometry(s), m_index(this), m_vertexColor(this)
{
  this->m_vertexAttributes.emplace("position", this);
  this->m_vertexAttributes.emplace("normal", this);
  this->m_vertexAttributes.emplace("tangent", this);
  this->m_vertexAttributes.emplace("color", this);
  this->m_vertexAttributes.emplace("attribute0", this);
  this->m_vertexAttributes.emplace("attribute1", this);
  this->m_vertexAttributes.emplace("attribute2", this);
  this->m_vertexAttributes.emplace("attribute3", this);

  this->m_faceVaryingAttributes.emplace("normal", this);
  this->m_faceVaryingAttributes.emplace("tangent", this);
  this->m_faceVaryingAttributes.emplace("color", this);
  this->m_faceVaryingAttributes.emplace("attribute0", this);
  this->m_faceVaryingAttributes.emplace("attribute1", this);
  this->m_faceVaryingAttributes.emplace("attribute2", this);
  this->m_faceVaryingAttributes.emplace("attribute3", this);
}

void Triangle::commitParameters()
{
  // Stashing these in a ChangeObserverPtr means that commit will be
  // called again if the array contents change.
  this->m_index = getParamObject<Array1D>("primitive.index");
  for (auto &iter : this->m_vertexAttributes) {
    iter.second = getParamObject<Array1D>("vertex." + iter.first);
  }
  for (auto &iter : this->m_faceVaryingAttributes) {
    iter.second = getParamObject<Array1D>("faceVarying." + iter.first);
  }

  box1 range = {0, 1};
  this->getParam("valueRange", ANARI_FLOAT32_BOX1, &range);
  this->m_colorTable.RescaleToRange({range.lower, range.upper});
}

void Triangle::finalize()
{
  helium::ChangeObserverPtr<Array1D> &positionArray =
      this->vertexAttribute("position");
  if (!positionArray) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "'triangle' geometry missing 'vertex.position' parameter");
    return;
  }

  if (!this->m_index) {
    reportMessage(ANARI_SEVERITY_INFO, "generating 'triangle' index array");

    Array1DMemoryDescriptor md;
    md.appMemory = nullptr;
    md.deleter = nullptr;
    md.deleterPtr = nullptr;
    md.elementType = ANARI_UINT32_VEC3;
    md.numItems = positionArray->totalSize() / 3;

    this->m_index = new Array1D(this->deviceState(), md);
    this->m_index->refDec(helium::RefType::PUBLIC); // no public references

    auto *begin = (uint32_t *)this->m_index->map();
    auto *end = begin + this->m_index->totalSize() * 3;
    std::iota(begin, end, 0);
  }

  // Reset data
  this->m_dataSet = viskores::cont::DataSet{};
  this->m_mapper = std::make_shared<viskores::rendering::MapperRayTracer>();

  // Get the connection array.
  // Note that ANARI provides the connection array as a series of triples
  // whereas Viskores wants a flat array of indices. The easist way to do the
  // conversion (while sharing pointers) is to use ArrayHandleRuntimeVec.
  viskores::cont::ArrayHandleRuntimeVec<viskores::Id> connectionArray(3);
  viskores::cont::ArrayCopyShallowIfPossible(
      this->m_index->dataAsViskoresArray(), connectionArray);

  viskores::cont::CellSetSingleType<> cellSet;
  cellSet.Fill(static_cast<viskores::Id>(positionArray->size()),
      viskores::CELL_SHAPE_TRIANGLE,
      3,
      connectionArray.GetComponentsArray());
  this->m_dataSet.SetCellSet(cellSet);

  for (auto &iter : this->m_vertexAttributes) {
    if (iter.second) {
      this->m_dataSet.AddPointField(
          iter.first, iter.second->dataAsViskoresArray());
    }
  }
  for (auto &iter : this->m_faceVaryingAttributes) {
    if (iter.second) {
      this->m_dataSet.AddCellField(
          iter.first, iter.second->dataAsViskoresArray());
    }
  }

  // We have already checked that the position array exists.
  this->m_dataSet.AddCoordinateSystem("position");

  this->AddAttributeInformation();
}

bool Triangle::isValid() const
{
  return this->vertexAttribute("position");
}

} // namespace viskores_device
