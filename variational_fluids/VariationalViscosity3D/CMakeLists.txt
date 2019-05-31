add_executable(variational_viscosity_3d
  fluidsim.cpp
  levelset_util.cpp
  viscosity3d.cpp
  volume_fractions.cpp
  main.cpp
  )

target_link_libraries(variational_viscosity_3d
  PRIVATE
  nova::tools
  )

file( RELATIVE_PATH REL ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

install(TARGETS variational_viscosity_3d RUNTIME DESTINATION ${REL})
