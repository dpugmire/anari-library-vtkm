// Copyright 2022-2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "helium/array/Array.h"
// Viskores
#include <viskores/cont/UnknownArrayHandle.h>

namespace viskores_device {

/// @brief Convert an array provided by ANARI to an ArrayHandle managed by Viskores.
///
/// This routine allows you to take arrays of data provided in ANARI object parameters
/// and move them over to a Viskores ArrayHandle. Memory is shared between the two arrays,
/// so if the contents of the array is modified on one side, it can invalidate the other.
viskores::cont::UnknownArrayHandle ANARIArrayToViskoresArray(const helium::Array *anariArray);

/// @brief Convert an array of colors.
///
/// Given an array of values that are to represent colors, this function will look to see
/// if the array is represented by integer (fixed-point) values. In this case,
/// ANARI treats these values as color channels with the range of [0, MAX_VALUE]. Viskores
/// assumes colors are represented by floating point values in the range [0, 1]. This routine
/// will check for those cases and convert the values if necessary. If no conversion is
/// necessary, the same array will be returned.
viskores::cont::UnknownArrayHandle ANARIColorsToViskoresColors(
    const viskores::cont::UnknownArrayHandle &anariColors);

} // viskores_device
