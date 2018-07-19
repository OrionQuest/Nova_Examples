if(ENABLE_EMBEDDED_DEFORMABLES)
  add_executable(embedded_deformables_3d
    main.cpp
    ../Embedded_Deformables_Driver.cpp
    ../Embedded_Deformables_Example.cpp
    ../Rotated_Stress_Derivative.cpp
  )

  target_link_libraries(embedded_deformables_3d
    PRIVATE
    nova::geometry
    nova::tools
    )
  
  file(RELATIVE_PATH REL ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  
  install(TARGETS embedded_deformables_3d RUNTIME DESTINATION ${REL})
endif()
