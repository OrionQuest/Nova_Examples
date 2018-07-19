set(ENABLE_EMBEDDED_DEFORMABLES OFF CACHE BOOL "Enable building the embedded deformables project tree.")

if(ENABLE_EMBEDDED_DEFORMABLES)
  add_subdirectory(embedded_deformables_2d)
  add_subdirectory(embedded_deformables_3d)
  add_subdirectory(plugin_Embedded_Deformables)
endif()
