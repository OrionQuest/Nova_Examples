# Add the modules for this project tree
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

add_subdirectory(opengl)
add_subdirectory(embedded_deformables)
add_subdirectory(variational_fluids)
