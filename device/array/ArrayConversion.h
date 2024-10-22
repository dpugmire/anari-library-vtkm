// Copyright 2022-2024 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "helium/array/Array.h"
// VTK-m
#include <vtkm/cont/UnknownArrayHandle.h>

namespace vtkm_device {

vtkm::cont::UnknownArrayHandle ANARIArrayToVTKmArray(const helium::Array *anariArray);

} // vtkm_device
