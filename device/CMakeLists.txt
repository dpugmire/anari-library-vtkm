
## Code generation ##

anari_generate_queries(
  NAME vtkm_device
  PREFIX VTKmDevice
  CPP_NAMESPACE vtkm_device
  JSON_DEFINITIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vtkm_device.json
)

## Build device target ##

add_library(${PROJECT_NAME} SHARED
  VTKmDevice.cpp
  VTKmDeviceQueries.cpp
  VTKmDeviceGlobalState.cpp
  VTKmLibrary.cpp
  Object.cpp
  array/Array1D.cpp
  array/Array2D.cpp
  array/Array3D.cpp
  array/ObjectArray.cpp
  camera/Camera.cpp
  frame/Frame.cpp
  renderer/Renderer.cpp
  scene/Group.cpp
  scene/Instance.cpp
  scene/World.cpp
  scene/light/Light.cpp
  scene/surface/Surface.cpp
  scene/surface/geometry/Geometry.cpp
  scene/surface/material/Material.cpp
  scene/surface/material/sampler/Sampler.cpp
  scene/volume/Volume.cpp
  scene/volume/spatial_field/SpatialField.cpp
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
  anari::helium
  vtkm::worklet
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
  EXPORT_MACRO_NAME "VTKM_LIBRARY_INTERFACE"
)

target_include_directories(${PROJECT_NAME}
PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

## Installation ##

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
