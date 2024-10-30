// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "VTKmDevice.h"

#include "array/Array1D.h"
#include "array/Array2D.h"
#include "array/Array3D.h"
#include "array/ObjectArray.h"
#include "frame/Frame.h"
#include "scene/volume/spatial_field/SpatialField.h"

#include "VTKmDeviceQueries.h"

namespace vtkm_device {

///////////////////////////////////////////////////////////////////////////////
// Helper functions ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename HANDLE_T, typename OBJECT_T>
inline HANDLE_T getHandleForAPI(OBJECT_T *object)
{
  return (HANDLE_T)object;
}

template <typename OBJECT_T, typename HANDLE_T, typename... Args>
inline HANDLE_T createObjectForAPI(VTKmDeviceGlobalState *s, Args &&...args)
{
  return getHandleForAPI<HANDLE_T>(
      new OBJECT_T(s, std::forward<Args>(args)...));
}

///////////////////////////////////////////////////////////////////////////////
// VTKmDevice definitions /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Data Arrays ////////////////////////////////////////////////////////////////

void *VTKmDevice::mapArray(ANARIArray a)
{
  deviceState()->renderingSemaphore.arrayMapAcquire();
  return helium::BaseDevice::mapArray(a);
}

void VTKmDevice::unmapArray(ANARIArray a)
{
  helium::BaseDevice::unmapArray(a);
  deviceState()->renderingSemaphore.arrayMapRelease();
}

// API Objects ////////////////////////////////////////////////////////////////

ANARIArray1D VTKmDevice::newArray1D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems)
{
  initDevice();

  Array1DMemoryDescriptor md;
  md.appMemory = appMemory;
  md.deleter = deleter;
  md.deleterPtr = userData;
  md.elementType = type;
  md.numItems = numItems;

  if (anari::isObject(type))
    return createObjectForAPI<ObjectArray, ANARIArray1D>(deviceState(), md);
  else
    return createObjectForAPI<Array1D, ANARIArray1D>(deviceState(), md);
}

ANARIArray2D VTKmDevice::newArray2D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2)
{
  initDevice();

  Array2DMemoryDescriptor md;
  md.appMemory = appMemory;
  md.deleter = deleter;
  md.deleterPtr = userData;
  md.elementType = type;
  md.numItems1 = numItems1;
  md.numItems2 = numItems2;

  return createObjectForAPI<Array2D, ANARIArray2D>(deviceState(), md);
}

ANARIArray3D VTKmDevice::newArray3D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t numItems3)
{
  initDevice();

  Array3DMemoryDescriptor md;
  md.appMemory = appMemory;
  md.deleter = deleter;
  md.deleterPtr = userData;
  md.elementType = type;
  md.numItems1 = numItems1;
  md.numItems2 = numItems2;
  md.numItems3 = numItems3;

  return createObjectForAPI<Array3D, ANARIArray3D>(deviceState(), md);
}

ANARICamera VTKmDevice::newCamera(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARICamera>(
      Camera::createInstance(subtype, deviceState()));
}

ANARIFrame VTKmDevice::newFrame()
{
  initDevice();
  return createObjectForAPI<Frame, ANARIFrame>(deviceState());
}

ANARIGeometry VTKmDevice::newGeometry(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARIGeometry>(
      Geometry::createInstance(subtype, deviceState()));
}

ANARIGroup VTKmDevice::newGroup()
{
  initDevice();
  return createObjectForAPI<Group, ANARIGroup>(deviceState());
}

ANARIInstance VTKmDevice::newInstance(const char * /*subtype*/)
{
  initDevice();
  return createObjectForAPI<Instance, ANARIInstance>(deviceState());
}

ANARILight VTKmDevice::newLight(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARILight>(
      Light::createInstance(subtype, deviceState()));
}

ANARIMaterial VTKmDevice::newMaterial(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARIMaterial>(
      Material::createInstance(subtype, deviceState()));
}

ANARIRenderer VTKmDevice::newRenderer(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARIRenderer>(
      Renderer::createInstance(subtype, deviceState()));
}

ANARISampler VTKmDevice::newSampler(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARISampler>(
      Sampler::createInstance(subtype, deviceState()));
}

ANARISpatialField VTKmDevice::newSpatialField(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARISpatialField>(
      SpatialField::createInstance(subtype, deviceState()));
}

ANARISurface VTKmDevice::newSurface()
{
  initDevice();
  return createObjectForAPI<Surface, ANARISurface>(deviceState());
}

ANARIVolume VTKmDevice::newVolume(const char *subtype)
{
  initDevice();
  return getHandleForAPI<ANARIVolume>(
      Volume::createInstance(subtype, deviceState()));
}

ANARIWorld VTKmDevice::newWorld()
{
  initDevice();
  return createObjectForAPI<World, ANARIWorld>(deviceState());
}

// Query functions ////////////////////////////////////////////////////////////

const char **VTKmDevice::getObjectSubtypes(ANARIDataType objectType)
{
  return vtkm_device::query_object_types(objectType);
}

const void *VTKmDevice::getObjectInfo(ANARIDataType objectType,
    const char *objectSubtype,
    const char *infoName,
    ANARIDataType infoType)
{
  return vtkm_device::query_object_info(
      objectType, objectSubtype, infoName, infoType);
}

const void *VTKmDevice::getParameterInfo(ANARIDataType objectType,
    const char *objectSubtype,
    const char *parameterName,
    ANARIDataType parameterType,
    const char *infoName,
    ANARIDataType infoType)
{
  return vtkm_device::query_param_info(objectType,
      objectSubtype,
      parameterName,
      parameterType,
      infoName,
      infoType);
}

// Object + Parameter Lifetime Management /////////////////////////////////////

int VTKmDevice::getProperty(ANARIObject object,
    const char *name,
    ANARIDataType type,
    void *mem,
    uint64_t size,
    uint32_t mask)
{
  if (mask == ANARI_WAIT) {
    auto lock = scopeLockObject();
    deviceState()->waitOnCurrentFrame();
  }

  return helium::BaseDevice::getProperty(object, name, type, mem, size, mask);
}

// Other VTKmDevice definitions /////////////////////////////////////////////

VTKmDevice::VTKmDevice(ANARIStatusCallback cb, const void *ptr)
    : helium::BaseDevice(cb, ptr)
{
  m_state = std::make_unique<VTKmDeviceGlobalState>(this_device());
  deviceCommitParameters();
}

VTKmDevice::VTKmDevice(ANARILibrary l) : helium::BaseDevice(l)
{
  m_state = std::make_unique<VTKmDeviceGlobalState>(this_device());
  deviceCommitParameters();
}

VTKmDevice::~VTKmDevice()
{
  reportMessage(ANARI_SEVERITY_DEBUG, "destroying VTKm device (%p)", this);

  auto &state = *deviceState();
  state.commitBufferClear();
}

void VTKmDevice::initDevice()
{
  if (m_initialized)
    return;

  reportMessage(ANARI_SEVERITY_DEBUG, "initializing VTKm device (%p)", this);

  m_initialized = true;
}

void VTKmDevice::deviceCommitParameters()
{
  helium::BaseDevice::deviceCommitParameters();
}

int VTKmDevice::deviceGetProperty(
    const char *name, ANARIDataType type, void *mem, uint64_t size)
{
  std::string_view prop = name;
  if (prop == "extension" && type == ANARI_STRING_LIST) {
    helium::writeToVoidP(mem, query_extensions());
    return 1;
  } else if (prop == "vtkm" && type == ANARI_BOOL) {
    helium::writeToVoidP(mem, true);
    return 1;
  }
  return 0;
}

VTKmDeviceGlobalState *VTKmDevice::deviceState() const
{
  return (VTKmDeviceGlobalState *)helium::BaseDevice::m_state.get();
}

} // namespace vtkm_device
