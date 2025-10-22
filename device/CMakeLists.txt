set(anari_viskores_target anari_library_viskores)

## Build device target ##

add_library(${anari_viskores_target} SHARED
  ViskoresDevice.cpp
  ViskoresDeviceGlobalState.cpp
  ViskoresLibrary.cpp
  Object.cpp
  array/Array1D.cpp
  array/Array2D.cpp
  array/Array3D.cpp
  array/ArrayConversion.cpp
  array/ObjectArray.cpp
  camera/Camera.cpp
  camera/Orthographic.cpp
  camera/Perspective.cpp
  frame/Frame.cpp
  renderer/Renderer.cpp
  scene/Group.cpp
  scene/Instance.cpp
  scene/World.cpp
  scene/light/Light.cpp
  scene/surface/Surface.cpp
  scene/surface/geometry/Geometry.cpp
  scene/surface/geometry/Triangle.cpp
  scene/surface/material/Material.cpp
  scene/surface/material/sampler/Sampler.cpp
  scene/volume/TransferFunction1D.cpp
  scene/volume/Volume.cpp
  scene/volume/spatial_field/SpatialField.cpp
  scene/volume/spatial_field/StructuredRegularField.cpp
#  scene/volume/TransferFunction1D.cpp
)

target_link_libraries(${anari_viskores_target}
PRIVATE
  anari::helium
  viskores::cont
  viskores::worklet
  viskores::source
  viskores::rendering
)

include(GenerateExportHeader)
generate_export_header(${anari_viskores_target}
  EXPORT_MACRO_NAME "VISKORES_LIBRARY_INTERFACE"
)

target_include_directories(${anari_viskores_target}
PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

## Code generation ##

anari_generate_queries(
  DEVICE_TARGET ${anari_viskores_target}
  CPP_NAMESPACE viskores_device
  JSON_DEFINITIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/viskores_device.json
)

## Installation ##

install(TARGETS ${anari_viskores_target}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
