# anari-library-vtkm

Proof-of-concept ANARI device implementation built using VTK-m.

## Build + install

Building this library requires the following:

- CMake 3.18+
- C++17 compiler
- [VTK-m](https://github.com/Kitware/VTK-m) master
- [ANARI-SDK](https://github.com/KhronosGroup/ANARI-SDK) 0.8.0+

VTK-m and ANARI-SDK can be found via placing their installation locations on
`CMAKE_PREFIX_PATH`.

The ANARI-SDK only requires that the resulting library is available on the
runtime library search path (i.e. `LD_LIBRARY_PATH` on Linux). Alternatively,
setting `CMAKE_INSTALL_PREFIX` to where ANARI-SDK is installed will install the
VTK-m device library next to `libanari`, which will also find it instead of
using a runtime library search path.
