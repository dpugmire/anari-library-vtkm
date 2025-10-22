set(anari_vtkm_target anari_library_vtkm)

## Build device target ##

add_library(${anari_vtkm_target} SHARED
  VTKmDevice.cpp
  VTKmDeviceGlobalState.cpp
  VTKmLibrary.cpp
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

target_link_libraries(${anari_vtkm_target}
PRIVATE
  anari::helium
  vtkm::cont
  vtkm::worklet
  vtkm::source
  vtkm::rendering
)

include(GenerateExportHeader)
generate_export_header(${anari_vtkm_target}
  EXPORT_MACRO_NAME "VTKM_LIBRARY_INTERFACE"
)

target_include_directories(${anari_vtkm_target}
PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

## Code generation ##

anari_generate_queries(
  DEVICE_TARGET ${anari_vtkm_target}
  CPP_NAMESPACE vtkm_device
  JSON_DEFINITIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vtkm_device.json
)

## Installation ##

install(TARGETS ${anari_vtkm_target}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
