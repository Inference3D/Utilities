#--------------------------------------------------------
# CMake for generating the VidExtractLib
#
# @author: Wild Boar
#
# @date: 2022-03-24
#--------------------------------------------------------

# Setup the folders
include_directories( "../" )

# Create Library
add_library(VidExtractLib SHARED
    Module.cpp
)

target_link_libraries(VidExtractLib NVLib ${OpenCV_LIBS} ModuleLib zip)
