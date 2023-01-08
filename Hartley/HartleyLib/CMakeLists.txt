#--------------------------------------------------------
# CMake for generating the HartleyLib
#
# @author: Wild Boar
#
# @date: 2022-10-09
#--------------------------------------------------------

# Setup the folders
include_directories( "../" )

# Create Library
add_library(HartleyLib SHARED
    Module.cpp
    Hartley.cpp
    Runner.cpp
)

target_link_libraries(HartleyLib NVLib ${OpenCV_LIBS} ModuleLib zip)
