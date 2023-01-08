#--------------------------------------------------------
# CMake for generating the main executable
#
# @author: Wild Boar
#
# @date: 2022-10-09
#--------------------------------------------------------

# Setup the folders
include_directories( "../" "${LIBRARY_BASE}/NVLib" )

# Create the executable
add_executable(Hartley
    Helpers/ArgUtils.cpp
    Engine.cpp
    Source.cpp
)

# Add link libraries                               
target_link_libraries(Hartley NVLib ModuleLib ${OpenCV_LIBS} ${CMAKE_DL_LIBS} OpenSSL::SSL uuid zip)

# Copy Resources across
add_custom_target(resource_copy ALL
	COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Hartley
    DEPENDS ${MY_TARGET}
)