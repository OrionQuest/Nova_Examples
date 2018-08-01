set( ENABLE_GRID_PLUGIN OFF CACHE BOOL "Enable Grid Renderable plugin." )

if( ENABLE_GRID_PLUGIN )
  
  add_library( plugin_Grid
    GridRenderable.cpp
    )

  if(WIN32)
    target_compile_definitions( plugin_Grid PUBLIC OPENGL_3D_WIN32=1)
  endif()
  
  if(UNIX)
    target_compile_definitions( plugin_Grid PUBLIC OPENGL_3D_LINUX=1)
  endif()
  
  
  target_link_libraries( plugin_Grid
    PRIVATE
    nova::tools
    ${Boost_LIBRARIES}
    opengl_kernel
    )
  
  target_compile_features( plugin_Grid
    PRIVATE
    cxx_auto_type
    )
endif()

