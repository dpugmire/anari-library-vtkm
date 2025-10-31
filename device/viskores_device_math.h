// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

// helium
#include "helium/helium_math.h"
// viskores
#include <viskores/Matrix.h>
#include <viskores/VectorAnalysis.h>
#include <viskores/interop/anari/ViskoresANARITypes.h>

namespace viskores_device {

using namespace anari::math;
using namespace helium::math;

using Mat3f_32 = viskores::Matrix<viskores::Float32, 3, 3>;
using Mat4f_32 = viskores::Matrix<viskores::Float32, 4, 4>;

template <typename T, viskores::IdComponent NumRow, viskores::IdComponent NumCol>
inline viskores::Matrix<T, NumRow, NumCol> toViskoresMatrix(mat<T, NumRow, NumCol>& anarimat)
{
  viskores::Matrix<T, NumRow, NumCol> viskoresmat;
  for (viskores::IdComponent row = 0; row < NumRow; ++row) {
    for (viskores::IdComponent col = 0; col < NumCol; ++col) {
      viskoresmat[row][col] = anarimat[col][row];
    }
  }
  return viskoresmat;
}

} // namespace viskores_device
