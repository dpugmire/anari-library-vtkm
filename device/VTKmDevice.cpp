// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "VTKmDevice.h"

#include "anari/anari_cpp.hpp"

#include <cstring>

namespace vtkm_device {

///////////////////////////////////////////////////////////////////////////////
// VTKmDevice definitions /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Data Arrays ////////////////////////////////////////////////////////////////

void managed_deleter(const void *, const void *memory)
{
  delete[] static_cast<char *>(const_cast<void *>(memory));
}

ANARIArray1D VTKmDevice::newArray1D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems)
{
  ANARIArray1D handle = nextHandle<ANARIArray1D>();
  if (auto obj = getObject(handle)) {
    if (appMemory == nullptr) {
      obj->userdata = nullptr;
      obj->memory = new char[sizeOf(type) * numItems];
      obj->deleter = managed_deleter;
    } else {
      obj->userdata = userData;
      obj->memory = appMemory;
      obj->deleter = deleter;
    }
  }
  return handle;
}

ANARIArray2D VTKmDevice::newArray2D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2)
{
  ANARIArray2D handle = nextHandle<ANARIArray2D>();
  if (auto obj = getObject(handle)) {
    if (appMemory == nullptr) {
      obj->userdata = nullptr;
      obj->memory = new char[sizeOf(type) * numItems1 * numItems2];
      obj->deleter = managed_deleter;
    } else {
      obj->userdata = userData;
      obj->memory = appMemory;
      obj->deleter = deleter;
    }
  }
  return handle;
}

ANARIArray3D VTKmDevice::newArray3D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t numItems3)
{
  ANARIArray3D handle = nextHandle<ANARIArray3D>();
  if (auto obj = getObject(handle)) {
    if (appMemory == nullptr) {
      obj->userdata = nullptr;
      obj->memory = new char[sizeOf(type) * numItems1 * numItems2 * numItems3];
      obj->deleter = managed_deleter;
    } else {
      obj->userdata = userData;
      obj->memory = appMemory;
      obj->deleter = deleter;
    }
  }
  return handle;
}

void *VTKmDevice::mapArray(ANARIArray a)
{
  if (auto obj = getObject(a)) {
    return const_cast<void *>(obj->memory);
  } else {
    return nullptr;
  }
}

void VTKmDevice::unmapArray(ANARIArray) {}

// Renderable Objects /////////////////////////////////////////////////////////

ANARILight VTKmDevice::newLight(const char *)
{
  return nextHandle<ANARILight>();
}

ANARICamera VTKmDevice::newCamera(const char *)
{
  return nextHandle<ANARICamera>();
}

ANARIGeometry VTKmDevice::newGeometry(const char *)
{
  return nextHandle<ANARIGeometry>();
}

ANARISpatialField VTKmDevice::newSpatialField(const char *)
{
  return nextHandle<ANARISpatialField>();
}

ANARISurface VTKmDevice::newSurface()
{
  return nextHandle<ANARISurface>();
}

ANARIVolume VTKmDevice::newVolume(const char *)
{
  return nextHandle<ANARIVolume>();
}

// Model Meta-Data ////////////////////////////////////////////////////////////

ANARIMaterial VTKmDevice::newMaterial(const char *)
{
  return nextHandle<ANARIMaterial>();
}

ANARISampler VTKmDevice::newSampler(const char *)
{
  return nextHandle<ANARISampler>();
}

// Instancing /////////////////////////////////////////////////////////////////

ANARIGroup VTKmDevice::newGroup()
{
  return nextHandle<ANARIGroup>();
}

ANARIInstance VTKmDevice::newInstance(const char *type)
{
  return nextHandle<ANARIInstance>();
}

// Top-level Worlds ///////////////////////////////////////////////////////////

ANARIWorld VTKmDevice::newWorld()
{
  return nextHandle<ANARIWorld>();
}

int VTKmDevice::getProperty(
    ANARIObject, const char *, ANARIDataType, void *, uint64_t, ANARIWaitMask)
{
  return 0;
}

const char **query_object_types(ANARIDataType type);
const void *query_object_info(ANARIDataType type,
    const char *subtype,
    const char *infoName,
    ANARIDataType infoType);
const void *query_param_info(ANARIDataType type,
    const char *subtype,
    const char *paramName,
    ANARIDataType paramType,
    const char *infoName,
    ANARIDataType infoType);

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

struct FrameData
{
  uint32_t width = 1;
  uint32_t height = 1;
};

void frame_deleter(const void *userdata, const void *memory)
{
  delete[] static_cast<char *>(const_cast<void *>(memory));
  delete static_cast<FrameData *>(const_cast<void *>(userdata));
}

void VTKmDevice::setParameter(
    ANARIObject object, const char *name, ANARIDataType type, const void *mem)
{
  if (auto obj = getObject(object)) {
    if (obj->type == ANARI_FRAME) {
      FrameData *data =
          static_cast<FrameData *>(const_cast<void *>(obj->userdata));
      if (type == ANARI_UINT32_VEC2 && std::strncmp("size", name, 4) == 0) {
        const uint32_t *size = static_cast<const uint32_t *>(mem);
        data->width = size[0];
        data->height = size[1];
        delete[] static_cast<char *>(const_cast<void *>(obj->memory));
        obj->memory = nullptr;
      }
    }
  }
}

void VTKmDevice::unsetParameter(ANARIObject, const char *) {}

void VTKmDevice::unsetAllParameters(ANARIObject) {}

void *VTKmDevice::mapParameterArray1D(ANARIObject object,
    const char *name,
    ANARIDataType dataType,
    uint64_t numElements1,
    uint64_t *elementStride)
{
  if (auto obj = getObject(object)) {
    if (elementStride) {
      *elementStride = 0;
    }
    return obj->mapArray(name, sizeOf(dataType) * numElements1);
  } else {
    return nullptr;
  }
}

void *VTKmDevice::mapParameterArray2D(ANARIObject object,
    const char *name,
    ANARIDataType dataType,
    uint64_t numElements1,
    uint64_t numElements2,
    uint64_t *elementStride)
{
  if (auto obj = getObject(object)) {
    if (elementStride) {
      *elementStride = 0;
    }
    return obj->mapArray(name, sizeOf(dataType) * numElements1 * numElements2);
  } else {
    return nullptr;
  }
}

void *VTKmDevice::mapParameterArray3D(ANARIObject object,
    const char *name,
    ANARIDataType dataType,
    uint64_t numElements1,
    uint64_t numElements2,
    uint64_t numElements3,
    uint64_t *elementStride)
{
  if (auto obj = getObject(object)) {
    if (elementStride) {
      *elementStride = 0;
    }
    return obj->mapArray(
        name, sizeOf(dataType) * numElements1 * numElements2 * numElements3);
  } else {
    return nullptr;
  }
}

void VTKmDevice::unmapParameterArray(ANARIObject object, const char *name) {}

void VTKmDevice::commitParameters(ANARIObject) {}

void VTKmDevice::release(ANARIObject object)
{
  if (auto obj = getObject(object)) {
    obj->release();
  }
}

void VTKmDevice::retain(ANARIObject object)
{
  if (auto obj = getObject(object)) {
    obj->retain();
  }
}

// Frame Manipulation /////////////////////////////////////////////////////////

ANARIFrame VTKmDevice::newFrame()
{
  ANARIFrame frame = nextHandle<ANARIFrame>();
  if (auto obj = getObject(frame)) {
    obj->userdata = new FrameData();
    obj->deleter = frame_deleter;
  }
  return frame;
}

const void *VTKmDevice::frameBufferMap(ANARIFrame fb,
    const char *,
    uint32_t *width,
    uint32_t *height,
    ANARIDataType *pixelType)
{
  if (auto obj = getObject(fb)) {
    if (obj->type == ANARI_FRAME) {
      const FrameData *data = static_cast<const FrameData *>(obj->userdata);
      if (obj->memory == nullptr) {
        obj->memory = new char[data->width * data->height * 4 * sizeof(float)];
      }
      *width = data->width;
      *height = data->height;
      *pixelType = ANARI_FLOAT32;
      return obj->memory;
    }
  }
  return nullptr;
}

void VTKmDevice::frameBufferUnmap(ANARIFrame, const char *) {}

// Frame Rendering ////////////////////////////////////////////////////////////

ANARIRenderer VTKmDevice::newRenderer(const char *)
{
  return nextHandle<ANARIRenderer>();
}

void VTKmDevice::renderFrame(ANARIFrame) {}

int VTKmDevice::frameReady(ANARIFrame, ANARIWaitMask)
{
  return 1;
}

void VTKmDevice::discardFrame(ANARIFrame) {}

// Other VTKmDevice definitions ///////////////////////////////////////////////

VTKmDevice::VTKmDevice(ANARILibrary library) : DeviceImpl(library)
{
  nextHandle<ANARIObject>(); // insert a handle at 0
}

const char **query_extensions();

} // namespace vtkm_device
