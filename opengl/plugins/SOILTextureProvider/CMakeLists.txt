set( ENABLE_SOIL_PLUGIN OFF CACHE BOOL "Enable SOIL TextureProvider plugin.")

if( ENABLE_SOIL_PLUGIN )

  find_package(SOIL)

  if( NOT SOIL_FOUND )
    message( FATAL_ERROR "Cannot build SOIL plugin without the SOIL library present." )
  endif()
  
  add_library( plugin_SOILTextureProvider
    SOILTextureProvider.cpp
    )
  
  if(WIN32)
    target_compile_definitions( plugin_SOILTextureProvider PUBLIC OPENGL_3D_WIN32=1)
  endif()
  
  if(UNIX)
    target_compile_definitions( plugin_SOILTextureProvider PUBLIC OPENGL_3D_LINUX=1)
  endif()
  
  
  target_link_libraries( plugin_SOILTextureProvider
    PRIVATE
    SOIL
    )
  
  target_compile_features( plugin_SOILTextureProvider
    PRIVATE
    cxx_auto_type
    )
  
endif()

