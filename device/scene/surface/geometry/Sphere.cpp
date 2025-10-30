// Copyright 2021-2025 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Sphere.h"

#include <viskores/cont/ArrayCopy.h>
#include <viskores/cont/ArrayHandleIndex.h>

#include <numeric>

namespace viskores_device {

Sphere::Sphere(ViskoresDeviceGlobalState *s)
    : Geometry(s), m_index(this), m_vertexPosition(this), m_vertexRadius(this)
{}

void Sphere::commitParameters()
{
  // Geometry::commitParameters(); ???
  m_index = getParamObject<Array1D>("primitive.index");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexRadius = getParamObject<Array1D>("vertex.radius");
  /*
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");
  */
}

void Sphere::finalize()
{
  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on sphere geometry");
    return;
  }

  m_globalRadius = getParam<float>("radius", 0.01f);

  const float *radius = nullptr;
  if (m_vertexRadius)
    radius = m_vertexRadius->beginAs<float>();

  const auto numSpheres = m_index ? m_index->size() : m_vertexPosition->size();

  this->m_dataSet = viskores::cont::DataSet{};
  this->m_mapper = std::make_shared<viskores::rendering::MapperPoint>();

  if (!m_index) {
    reportMessage(ANARI_SEVERITY_INFO, "generating 'sphere' index array");

    Array1DMemoryDescriptor md;
    md.appMemory = nullptr;
    md.deleter = nullptr;
    md.deleterPtr = nullptr;
    md.elementType = ANARI_UINT32_VEC3;
    md.numItems = this->m_vertexPosition->totalSize();

    this->m_index = new Array1D(this->deviceState(), md);
    this->m_index->refDec(
        helium::RefType::PUBLIC); // no public referencesdex->map();

    auto *begin = (uint32_t *)this->m_index->map();
    auto *end = begin + numSpheres;
    std::iota(begin, end, 0);
  }

  viskores::cont::UnknownArrayHandle viskoresArray =
      this->m_vertexRadius->dataAsViskoresArray();
  if (!viskoresArray.IsValueType<viskores::Float32>()
      && !viskoresArray.IsValueType<viskores::Float64>()) {
    viskores::cont::ArrayHandle<viskores::FloatDefault> castArray;
    viskores::cont::ArrayCopy(viskoresArray, castArray);
    viskoresArray = castArray;
  }

  this->m_dataSet.AddCoordinateSystem(
      {"coords", this->m_vertexPosition->dataAsViskoresArray()});

  auto connIdx = viskores::cont::make_ArrayHandleIndex(numSpheres);
  viskores::cont::ArrayHandle<viskores::Id> conn;
  viskores::cont::ArrayCopy(connIdx, conn);

  viskores::cont::CellSetSingleType<> cellSet;
  cellSet.Fill(static_cast<viskores::Id>(numSpheres),
      viskores::CELL_SHAPE_VERTEX,
      1,
      conn);
  this->m_dataSet.SetCellSet(cellSet);
  this->m_dataSet.AddPointField("data", viskoresArray);

  /*
  this->m_actor =
      std::make_shared<viskores::rendering::Actor>(this->m_dataSet.GetCellSet(),
          this->m_dataSet.GetCoordinateSystem(),
          this->m_dataSet.GetField("data"),
          this->m_colorTable);
          */
}

} // namespace viskores_device
