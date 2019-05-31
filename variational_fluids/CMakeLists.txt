set(ENABLE_VARIATIONAL_FLUIDS OFF CACHE BOOL "Enable variational fluids project.")

cmake_policy(SET CMP0054 NEW)

if(ENABLE_VARIATIONAL_FLUIDS)
    add_subdirectory(VariationalViscosity3D)
    add_subdirectory(plugin_Variational_Fluids)
endif()
