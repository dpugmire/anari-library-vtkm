// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/Array1D.h"
#include "anari/frontend/type_utility.h"
// C++
#include <type_traits>

namespace {

template <typename BaseType, int NumComponents>
struct VTKmTypeImpl
{
  using type = vtkm::Vec<BaseType, static_cast<vtkm::IdComponent>(NumComponents)>;
};
template <typename BaseType>
struct VTKmTypeImpl<BaseType, 1>
{
  using type = BaseType;
};

template <int ANARIDataTypeId>
struct ANARIToVTKmTypeImpl
{
  using properties = anari::ANARITypeProperties<ANARIDataTypeId>;
  using type = typename VTKmTypeImpl<typename properties::base_type, properties::components>::type;
};

template <int ANARIDataTypeId>
using ANARIToVTKmType = typename ANARIToVTKmTypeImpl<ANARIDataTypeId>::type;

template <int ANARIDataTypeId>
struct ConstructArrayHandle
{
  using T = ANARIToVTKmType<ANARIDataTypeId>;
  vtkm::cont::UnknownArrayHandle operator()(const void *memory, vtkm::Id numValues)
  {
    return this->DoIt(memory, numValues, std::is_pointer<T>{});
  }

  vtkm::cont::UnknownArrayHandle DoIt(
      const void *memory, vtkm::Id numValues, std::false_type /*is_pointer*/)
  {
    // TODO: The ANARI interface generally passes arrays in the host memory space.
    // This can be really inefficient as data is pulled from GPU to CPU, passed to
    // ANARI, and then copied right back to the GPU. Need an extension to allow
    // client applications to pass device memory directly.
    return vtkm::cont::make_ArrayHandle(
        reinterpret_cast<const T *>(memory), numValues, vtkm::CopyFlag::Off);
  }

  vtkm::cont::UnknownArrayHandle DoIt(const void *, vtkm::Id, std::true_type /*is_pointer*/)
  {
    std::cout << "Cannot convert an array of type "
              << anari::ANARITypeProperties<ANARIDataTypeId>::type_name
              << " to VTK-m.\n";
    return vtkm::cont::UnknownArrayHandle{};
  }
};
template <>
struct ConstructArrayHandle<ANARI_UNKNOWN>
{
  vtkm::cont::UnknownArrayHandle operator()(const void *, vtkm::Id)
  {
    std::cout << "Cannot convert an array of type ANARI_UNKNOWN to VTK-m.\n";
    return vtkm::cont::UnknownArrayHandle{};
  }
};

} // anonymous namespace

namespace vtkm_device {

Array1D::Array1D(VTKmDeviceGlobalState *state, const Array1DMemoryDescriptor &d)
    : helium::Array1D(state, d)
{
  state->objectCounts.arrays++;
}

Array1D::~Array1D()
{
  asVTKmDeviceState(deviceState())->objectCounts.arrays--;
}

void Array1D::unmap()
{
  this->helium::Array1D::unmap();
  // Invalidate VTK-m ArrayHandle
  this->m_VTKmArray.ReleaseResources();
}

vtkm::cont::UnknownArrayHandle Array1D::dataAsVTKmArray() const
{
  if (!this->m_VTKmArray.IsValid())
  {
    // Pull data from ANARI into VTK-m.
    vtkm::Id numValues = this->size();
    const void *memory = this->begin();

    const_cast<Array1D *>(this)->m_VTKmArray =
        anari::anariTypeInvoke<vtkm::cont::UnknownArrayHandle, ConstructArrayHandle>(
            this->elementType(), memory, numValues);
  }

  return this->m_VTKmArray;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Array1D *);
