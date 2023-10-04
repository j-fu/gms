cmake_minimum_required(VERSION 3.9)

project(
  GMS 
  VERSION 1.0
  LANGUAGES C Fortran
  )

find_package(X11 REQUIRED)

include(GNUInstallDirs)

set(GMS_SOURCES 
  gmsdrv.h gmsstub.c
  gmsker.c 
  gmsx11.c 
  gmsps.c 
  gmsgl.c
  gmsregis.c 
  gmshpgl.c 
  gmssun.c 
  gmstek.c 
  gmsmsc.c 
  gmsbgi.c 
  )


add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/gms.c
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  DEPENDS ${GMS_SOURCES}
  COMMAND   cat ${GMS_SOURCES} > ${PROJECT_BINARY_DIR}/gms.c
  )

add_library(gms STATIC ${PROJECT_BINARY_DIR}/gms.c)

target_compile_definitions(gms PUBLIC X11=1)
set_target_properties(gms PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(gms PROPERTIES PUBLIC_HEADER gms.h)
set_target_properties(gms PROPERTIES INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR})


install(TARGETS gms 
  LIBRARY DESTINATION ${CMAKE_INSTALL_DIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

add_executable(tgmsc tgmsc.c)
target_link_libraries(tgmsc gms ${X11_LIBRARIES} m)

add_executable(tgmsf tgmsf.f)
target_link_libraries(tgmsf gms ${X11_LIBRARIES} m)
