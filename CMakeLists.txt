cmake_minimum_required(VERSION 3.9)

project(
    GMS 
    VERSION 1.0
    LANGUAGES C Fortran
)


include(GNUInstallDirs)

set(GMS_SOURCES 
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

add_library(gms STATIC
 ${GMS_SOURCES}
)

set_target_properties(gms PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(gms PROPERTIES PUBLIC_HEADER gms.h)


install(TARGETS gms 
    LIBRARY DESTINATION ${CMAKE_INSTALL_DIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

