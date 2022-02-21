cmake_minimum_required(VERSION 3.1.0)
project(simpleGL)

#--- Print CMake infomation
message(STATUS "CMake version: ${CMAKE_VERSION}")

#--- Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--- Where to find the FindPackageName.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

#--- Where exes/libs will be deployed
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#--- Set compiler options
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

#--- Find QT
find_package(Qt5 COMPONENTS Core Gui Qml Quick Widgets REQUIRED)

#--- Configure QT
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

#--- Collect external header only libraries
find_package(Eigen3 REQUIRED)
find_package(libigl REQUIRED)

#--- Configure jsoncpp
set(JSONCPP_ROOT_DIRS ${CMAKE_SOURCE_DIR}/libraries/jsoncpp)
include(cmake/ConfigureJsoncpp.cmake)

#--- Configure assimp
set(ASSIMP_ROOT_DIRS ${CMAKE_SOURCE_DIR}/libraries/assimp)
include(cmake/ConfigureAssimp.cmake)

add_subdirectory(SimpleGL)