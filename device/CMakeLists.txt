
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
  VTKmLibrary.cpp
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

## Installation ##

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
