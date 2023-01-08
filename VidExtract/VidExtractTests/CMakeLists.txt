#--------------------------------------------------------
# CMake for generating VidExtract Testing Suite
#
# @author: Wild Boar
#
# @date: 2022-03-24
#--------------------------------------------------------

# Add Google Test to the test group
find_package(GTest REQUIRED)

# Setup the folders
include_directories( "../" "${LIBRARY_BASE}/NVLib" )

# Add the UnitTestLib library to the folder
add_library(UnitTestLib STATIC IMPORTED)
set_target_properties(UnitTestLib PROPERTIES
    IMPORTED_LOCATION "${LIBRARY_BASE}/UnitTestLib/build/UnitTestLib/libUnitTestLib.a"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBRARY_BASE}/UnitTestLib"
)

# Create the executable
add_executable(VidExtractTests
    Tests/Module_Test.cpp
)

# Add link libraries
target_link_libraries(VidExtractTests NVLib ${OpenCV_LIBS} ModuleLib ${CMAKE_DL_LIBS} UnitTestLib GTest::Main)

# Find the associated unit tests
gtest_discover_tests(VidExtractTests)
