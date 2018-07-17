set( ENABLE_ASSIMP_PLUGIN OFF CACHE BOOL "Enable Assimp Renderable plugin." )

if( ENABLE_ASSIMP_PLUGIN )
  
  find_package(assimp)

  if(NOT assimp_FOUND)
    message( FATAL_ERROR "Cannot build Assimp plugin without the Assimp library present." )
  endif()
  
  add_library( plugin_Assimp
    AssimpRenderable.cpp
    AssimpRenderable_Model.cpp
    AssimpRenderable_Mesh.cpp
    )

  add_subdirectory( miniball )

  if(WIN32)
    target_compile_definitions( plugin_Assimp PUBLIC OPENGL_3D_WIN32=1)
  endif()
  
  if(UNIX)
    target_compile_definitions( plugin_Assimp PUBLIC OPENGL_3D_LINUX=1)
  endif()
  
  
  target_link_libraries( plugin_Assimp
    PRIVATE
    ${Boost_LIBRARIES}
    ${assimp_LIBRARIES}
    plugin_Assimp_miniball
    )
  
  target_compile_features( plugin_Assimp
    PRIVATE
    cxx_auto_type
    )
endif()

