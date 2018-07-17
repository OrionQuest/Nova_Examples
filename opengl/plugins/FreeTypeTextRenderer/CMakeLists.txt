set( ENABLE_FREETYPE_PLUGIN OFF CACHE BOOL "Enable FreeType TextRenderer plugin." )

if( ENABLE_FREETYPE_PLUGIN )

  find_package( Freetype )

  if( NOT FREETYPE_FOUND )
    message( FATAL_ERROR "Cannot build FreeType plugin without the FreeType library present." )
  endif()
  
  add_library( plugin_FreeTypeRenderer
    FreeTypeRenderer.cpp
    )
  
  if(WIN32)
    target_compile_definitions( plugin_FreeTypeRenderer PUBLIC OPENGL_3D_WIN32=1)
  endif()
  
  if(UNIX)
    target_compile_definitions( plugin_FreeTypeRenderer PUBLIC OPENGL_3D_LINUX=1)
  endif()
  
  target_include_directories(plugin_FreeTypeRenderer PRIVATE
    ${FREETYPE_INCLUDE_DIRS}
    )
  
  target_link_libraries( plugin_FreeTypeRenderer
    PRIVATE
    ${FREETYPE_LIBRARIES}
    )
  
  target_compile_features( plugin_FreeTypeRenderer
    PRIVATE
    cxx_auto_type
    )

endif()
