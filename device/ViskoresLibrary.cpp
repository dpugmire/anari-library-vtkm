// Copyright 2023 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "ViskoresDevice.h"
#include "anari/backend/LibraryImpl.h"
#include "anari_library_viskores_export.h"

#include <viskores/cont/Initialize.h>

namespace viskores_device {

struct ViskoresLibrary : public anari::LibraryImpl
{
  ViskoresLibrary(
      void *lib, ANARIStatusCallback defaultStatusCB, const void *statusCBPtr);

  ANARIDevice newDevice(const char *subtype) override;
  const char **getDeviceExtensions(const char *deviceType) override;
};

// Definitions ////////////////////////////////////////////////////////////////

ViskoresLibrary::ViskoresLibrary(
    void *lib, ANARIStatusCallback defaultStatusCB, const void *statusCBPtr)
    : anari::LibraryImpl(lib, defaultStatusCB, statusCBPtr)
{
  if (!viskores::cont::IsInitialized())
  {
    viskores::cont::Initialize();
  }
}

ANARIDevice ViskoresLibrary::newDevice(const char * /*subtype*/)
{
  return (ANARIDevice) new ViskoresDevice(this_library());
}

const char **ViskoresLibrary::getDeviceExtensions(const char * /*deviceType*/)
{
  return nullptr;
}

} // namespace viskores_device

// Define library entrypoint //////////////////////////////////////////////////

extern "C" VISKORES_LIBRARY_INTERFACE ANARI_DEFINE_LIBRARY_ENTRYPOINT(
    viskores, handle, scb, scbPtr)
{
  return (ANARILibrary) new viskores_device::ViskoresLibrary(handle, scb, scbPtr);
}
