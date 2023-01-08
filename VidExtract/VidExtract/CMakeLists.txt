#--------------------------------------------------------
# CMake for generating the main executable
#
# @author: Wild Boar
#
# @date: 2022-03-24
#--------------------------------------------------------

# Setup the folders
include_directories( "../" "${LIBRARY_BASE}/NVLib" )

# Create the executable
add_executable(VidExtract
    Engine.cpp
    Source.cpp
)

# Add link libraries                               
target_link_libraries(VidExtract NVLib ModuleLib ${OpenCV_LIBS} ${CMAKE_DL_LIBS} )

# Copy Resources across
add_custom_target(resource_copy ALL
	COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/VidExtract 
    DEPENDS ${MY_TARGET}
)
