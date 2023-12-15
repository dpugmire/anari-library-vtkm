// Copyright 2023 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "VTKmDevice.h"
#include "anari/backend/LibraryImpl.h"
#include "anari_library_vtkm_export.h"

namespace vtkm_device {

struct VTKmLibrary : public anari::LibraryImpl
{
  VTKmLibrary(
      void *lib, ANARIStatusCallback defaultStatusCB, const void *statusCBPtr);

  ANARIDevice newDevice(const char *subtype) override;
  const char **getDeviceExtensions(const char *deviceType) override;
};

// Definitions ////////////////////////////////////////////////////////////////

VTKmLibrary::VTKmLibrary(
    void *lib, ANARIStatusCallback defaultStatusCB, const void *statusCBPtr)
    : anari::LibraryImpl(lib, defaultStatusCB, statusCBPtr)
{}

ANARIDevice VTKmLibrary::newDevice(const char * /*subtype*/)
{
  return (ANARIDevice) new VTKmDevice(this_library());
}

const char **VTKmLibrary::getDeviceExtensions(const char * /*deviceType*/)
{
  return nullptr;
}

} // namespace vtkm_device

// Define library entrypoint //////////////////////////////////////////////////

extern "C" VTKM_LIBRARY_INTERFACE ANARI_DEFINE_LIBRARY_ENTRYPOINT(
    vtkm, handle, scb, scbPtr)
{
  return (ANARILibrary) new vtkm_device::VTKmLibrary(handle, scb, scbPtr);
}
