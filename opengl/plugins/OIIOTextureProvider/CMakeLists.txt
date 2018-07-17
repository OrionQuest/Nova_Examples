
set( ENABLE_OPENIMAGEIO_PLUGIN OFF CACHE BOOL "Enable OpenImageIO TextureProvider plugin." )

if( ENABLE_OPENIMAGEIO_PLUGIN )
  find_package(OIIO)

  if(NOT OPENIMAGEIO_FOUND )
    message( FATAL_ERROR "Cannot build OpenImageIO plugin without the OpenImageIO library present." )
  endif()

  add_library( plugin_OIIOTextureProvider
    OIIOTextureProvider.cpp
    )

  if(WIN32)
    target_compile_definitions( plugin_OIIOTextureProvider PUBLIC OPENGL_3D_WIN32=1)
  endif()

  if(UNIX)
    target_compile_definitions( plugin_OIIOTextureProvider PUBLIC OPENGL_3D_LINUX=1)
  endif()

  target_link_libraries( plugin_OIIOTextureProvider
    PRIVATE
    ${OPENIMAGEIO_LIBRARY}
    )

  target_compile_features( plugin_OIIOTextureProvider
    PRIVATE
    cxx_auto_type
    )
  
endif()
