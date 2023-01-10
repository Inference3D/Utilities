#--------------------------------------------------------
# CMake for generating the PatchMatchLib
#
# @author: Wild Boar
#
# Date Created: 2023-01-10
#--------------------------------------------------------

# Setup includes directories
include_directories("../" "${LIBRARY_BASE}/NVLib")

# Create Library
add_library(PatchMatchLib STATIC
    ArgUtils.cpp
    Calibration.cpp
    Frame.cpp
)
