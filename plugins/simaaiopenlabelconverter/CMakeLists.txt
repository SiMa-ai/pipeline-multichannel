# Copyright 2025 SiMa.ai
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.22)

set(plugin_version "1.7")
set(plugin_name "simaaiopenlabelconverter")
string(SUBSTRING ${plugin_name} 0 1 FIRST_CHAR)
string(TOUPPER ${FIRST_CHAR} FIRST_CHAR)
string(SUBSTRING ${plugin_name} 1 -1 REST_OF_STRING)
set(name_camel "${FIRST_CHAR}${REST_OF_STRING}")

set(PROJECT_NAME "gst${plugin_name}")

project("${PROJECT_NAME}"
  VERSION 0.1
  DESCRIPTION "Simaai ${PROJECT_NAME} plugin"
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT "$ENV{DEPENDENCIES_DIR_1}" STREQUAL "")
  set(TEMPLATES_DIR $ENV{DEPENDENCIES_DIR_1})
  set(CORE_ALLOCATOR_DIR $ENV{DEPENDENCIES_DIR_1}/allocator)
  set(CORE_BUFFER_POOL_DIR $ENV{DEPENDENCIES_DIR_1}/buffer-pool)
else()
  set(TEMPLATES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../templates)
  set(CORE_ALLOCATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../core/allocator)
  set(CORE_BUFFER_POOL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../core/buffer-pool)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_package(PkgConfig)
find_package(OpenCV REQUIRED)

pkg_check_modules(GLIB2 glib-2.0)
pkg_check_modules(GSTREAMER gstreamer-1.0)

if (NOT GLIB2_FOUND OR NOT GSTREAMER_FOUND)
    message(WARNING "${PROJECT_NAME} project is not configured due to absence of gstreamer component(s)" )
    message(WARNING "Please install GStreamer 1.14.0+ before running the sample." )
    return()
endif()

set(GLIBS
  ${GLIBS}
  glib-2.0
  gobject-2.0
  gio-2.0
  gstcontroller-1.0
  gstbase-1.0
  gstreamer-1.0
)

add_definitions(-DVERSION=\"${plugin_version}\")
add_definitions(-DGST_LICENSE=\"LGPL\")
add_definitions(-DGST_PACKAGE_NAME=\"GStreamer\ SiMa.ai\ ${PROJECT_NAME}\ Plug-in\")
add_definitions(-DGST_PACKAGE_ORIGIN=\"https://bitbucket.org/sima-ai/gst-plugins-sima\")
add_definitions(-DPACKAGE=\"gst-plugins-sima\")

add_definitions(-DPLUGIN_NAME_LOWER=${plugin_name})
add_definitions(-DPLUGIN_NAME_CAMEL=${name_camel})

add_library(${PROJECT_NAME}
  SHARED
  payload.cpp
)

include(GNUInstallDirs)

target_include_directories(${PROJECT_NAME} 
    PRIVATE
    .
    ${TEMPLATES_DIR}
    PUBLIC "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
    ${GLIB2_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CORE_ALLOCATOR_DIR}
    ${CORE_BUFFER_POOL_DIR}  
)

find_library(GLIB2_LIBRARY glib-2.0 PATHS ${GLIB2_LIBRARY_DIRS} )
find_library(GOBJECT2_LIBRARY gobject-2.0 PATHS ${GLIB2_LIBRARY_DIRS} )
find_library(GSTBASE_LIBRARY gstbase-1.0 PATHS ${GSTREAMER_LIBRARY_DIRS} )
find_library(GST_LIBRARY gstreamer-1.0 PATHS ${GSTREAMER_LIBRARY_DIRS} )

target_link_libraries(${PROJECT_NAME}
   PUBLIC 
   ${GLIB2_LIBRARY}
   ${GOBJECT2_LIBRARY}
   ${GSTBASE_LIBRARY}
   ${GST_LIBRARY}
   ${OpenCV_LIBS}
   simaaitrace
   simaailog
   gstsimaallocator
   gstsimaaibufferpool
)

INSTALL(TARGETS "${PROJECT_NAME}"  DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL(TARGETS "${PROJECT_NAME}"  DESTINATION ${CMAKE_INSTALL_LIBDIR}/gstreamer-1.0)

