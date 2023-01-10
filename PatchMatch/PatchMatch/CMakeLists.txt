#--------------------------------------------------------
# CMake for generating the main executable
#
# @author: Wild Boar
#
# Date Created: 2023-01-10
#--------------------------------------------------------

# Setup the includes
include_directories("../")

# Create the executable
add_executable(PatchMatch
    Engine.cpp
    Source.cpp
)

# Add link libraries                               
target_link_libraries(PatchMatch PatchMatchLib  NVLib ${OpenCV_LIBS})

# Copy Resources across
add_custom_target(resource_copy ALL
	COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/PatchMatch 
    DEPENDS ${MY_TARGET}
)
