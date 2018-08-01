set( ENABLE_TEXTFILERENDERER_PLUGIN OFF CACHE BOOL "Enable TextFileRenderer plugin.")

if( ENABLE_TEXTFILERENDERER_PLUGIN )
  
  add_library( plugin_TextFileRenderer
    TextFileRenderer.cpp
    )
  
  if(WIN32)
    target_compile_definitions( plugin_TextFileRenderer PUBLIC OPENGL_3D_WIN32=1)
  endif()
  
  if(UNIX)
    target_compile_definitions( plugin_TextFileRenderer PUBLIC OPENGL_3D_LINUX=1)
  endif()
  
  target_compile_features( plugin_TextFileRenderer
    PRIVATE
    cxx_auto_type
    )

  target_link_libraries( plugin_TextFileRenderer
    PRIVATE
    opengl_kernel
    )

endif()

