# CMakeLists.txt in X265v2 dir
cmake_minimum_required(VERSION 3.11)

# hints from https://github.com/robotology/how-to-export-cpp-library/blob/master/CMakeLists.txt
project(X265v2
        LANGUAGES CXX C
        VERSION 1.0.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")

IF(WIN32)
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_BUILD_FOR_SHORT")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
# get rid of some compiler warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
ELSE(WIN32)
add_definitions("-Wwrite-strings")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
ENDIF(WIN32)

include(FetchContent)

FetchContent_Declare(
  Vpp
  GIT_REPOSITORY https://github.com/CSIR-RTVC/vpp
)
# Declare dependencies
find_package(Vpp 1.0.0 REQUIRED)

set (x265_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vcpkg/installed/x86-windows/include)
set (x265_LIB_DIR ${PROJECT_SOURCE_DIR}/vcpkg/installed/x86-windows/lib)

SET(X265_LIB_HDRS
./include/X265v2/X265v2.h
./include/X265v2/X265v2Codec.h
./src/stdafx.h
)

SET(X265_LIB_SRCS 
./src/X265v2.cpp
./src/X265v2Codec.cpp
./src/stdafx.cpp
)

ADD_LIBRARY( X265v2 SHARED ${X265_LIB_SRCS} ${X265_LIB_HDRS})

target_include_directories(X265v2
    PUBLIC 
        $<INSTALL_INTERFACE:install>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/X265v2>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
		$<BUILD_INTERFACE:${x265_INCLUDE_DIR}>
)

#LINK_DIRECTORIES(
#    ${x265_LIB_DIR}
#)

TARGET_LINK_LIBRARIES(
X265v2
Vpp::Vpp
#${x265_LIB_DIR}/libx265
${x265_LIB_DIR}/x265-static.lib
) 

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/X265v2)

install(TARGETS X265v2
    EXPORT X265v2-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#Export the targets to a script
install(EXPORT X265v2-targets
  FILE
    X265v2Targets.cmake
  NAMESPACE
    X265v2::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/X265v2ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/X265v2Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/X265v2Config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/X265v2Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/X265v2ConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
## Exporting from the build tree

export(EXPORT X265v2-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/X265v2Targets.cmake NAMESPACE X265v2::)

#Register package in user's package registry
export(PACKAGE X265v2)

