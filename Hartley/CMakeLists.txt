#--------------------------------------------------------
# Top-Level: Hartley
#
# @author: Wild Boar
#
# @date: 2022-10-09
#--------------------------------------------------------

# Setup the project
cmake_minimum_required(VERSION 3.0.0)
project(Hartley VERSION 0.1.0)

# Setup base directory
set(LIBRARY_BASE $ENV{HOME}/Libraries)

# Setup the folders
include_directories( "${LIBRARY_BASE}/NVLib" "${LIBRARY_BASE}/ModuleLib")

# Add the NVLib library to the folder
add_library(NVLib STATIC IMPORTED)
set_target_properties(NVLib PROPERTIES
    IMPORTED_LOCATION "${LIBRARY_BASE}/NVLib/build/NVLib/libNVLib.a"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBRARY_BASE}/NVLib"
)

# Add the ModuleLib library to the folder
add_library(ModuleLib STATIC IMPORTED)
set_target_properties(ModuleLib PROPERTIES
    IMPORTED_LOCATION "${LIBRARY_BASE}/ModuleLib/build/ModuleLib/libModuleLib.a"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBRARY_BASE}/ModuleLib"
)

# Add OpenCV to the solution
find_package( OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Add OpenSSl
find_package(OpenSSL REQUIRED)

# Enable Testing
enable_testing()

# Setup the right version C++
set(CMAKE_CXX_STANDARD 17)

# Setup directories
add_subdirectory(HartleyLib)
add_subdirectory(HartleyTests)
add_subdirectory(Hartley)
