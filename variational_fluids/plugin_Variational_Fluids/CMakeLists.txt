if(ENABLE_VARIATIONAL_FLUIDS)
  find_package(assimp)

  if(NOT assimp_FOUND)
    message(FATAL_ERROR "Cannot build embedded deformables plugin without the Assimp library being present.")
  endif()
  
  add_library(plugin_Variational_Fluids
    ../../opengl/plugins/Assimp/AssimpRenderable_Model.cpp
    ../../opengl/plugins/Assimp/AssimpRenderable_Mesh.cpp
    main.cpp
    )

  if(WIN32)
    target_compile_definitions(plugin_Variational_Fluids PUBLIC OPENGL_3D_WIN32=1)
  endif()
  
  if(UNIX)
    target_compile_definitions(plugin_Variational_Fluids PUBLIC OPENGL_3D_LINUX=1)
  endif()

  target_include_directories(plugin_Variational_Fluids
                             PUBLIC ${VISUALIZER_KERNEL_INCLUDES})
  
  
  target_link_libraries(plugin_Variational_Fluids
    PRIVATE
    nova::geometry
    nova::tools
    ${Boost_LIBRARIES}
    ${assimp_LIBRARIES}
    plugin_Assimp_miniball
    )
  
  target_compile_features(plugin_Variational_Fluids
    PRIVATE
    cxx_auto_type
    )
endif()
