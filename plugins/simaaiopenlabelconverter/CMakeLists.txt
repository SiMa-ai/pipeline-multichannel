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

cmake_minimum_required(VERSION 3.16)

set(plugin_version "1.7")
set(plugin_name "simaaiopenlabelconverter")

string(SUBSTRING ${plugin_name} 0 1 FIRST_CHAR)
string(TOUPPER ${FIRST_CHAR} FIRST_CHAR)
string(SUBSTRING ${plugin_name} 1 -1 REST_OF_STRING)
set(name_camel "${FIRST_CHAR}${REST_OF_STRING}")

project("${plugin_name}"
    VERSION 0.1
    DESCRIPTION "SiMa.ai ${plugin_name} plugin"
    HOMEPAGE_URL "https://bitbucket.org/sima-ai/gst-simaai-plugins-base/"
    LANGUAGES C CXX
)

if (NOT "$ENV{DEPENDENCIES_DIR_1}" STREQUAL "")
  set(DEPENDENCIES_DIR_1 $ENV{DEPENDENCIES_DIR_1})
else()
  set(DEPENDENCIES_DIR_1 ${CMAKE_CURRENT_SOURCE_DIR}/../templates)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GSTREAMER_BASE REQUIRED IMPORTED_TARGET gstreamer-base-1.0)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(OPENCV REQUIRED IMPORTED_TARGET opencv4)
pkg_check_modules(JSON_GLIB json-glib-1.0)

add_definitions(
    -DVERSION=\"1.20.7\"
    -DGST_LICENSE=\"LGPL\"
    -DGST_PACKAGE_NAME=\"SiMa.ai\ ${plugin_name}\ plugin\"
    -DGST_PACKAGE_ORIGIN=\"https://bitbucket.org/sima-ai/gst-simaai-plugins-base/\"
    -DPACKAGE=\"gst-simaai-plugins-base\"
    -DPLUGIN_NAME_LOWER=${plugin_name}
    -DPLUGIN_NAME_CAMEL=${plugin_name}
)

add_library(${PROJECT_NAME}
    SHARED
    payload.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_DIR_1}
    PUBLIC
    ../../core/allocator
    ../../core/buffer-pool
    ../../core/caps
    ../../core/utils    
    ${JSON_GLIB_INCLUDE_DIRS}
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    PkgConfig::GSTREAMER
    PkgConfig::GSTREAMER_BASE
    PkgConfig::GLIB
    PkgConfig::OPENCV
    simaaitrace
    simaailog
    gstsimaallocator
    gstsimaaibufferpool
    gstsimaaicaps
    simaai_genboxdecode
)

include(GNUInstallDirs)

INSTALL(TARGETS "${PROJECT_NAME}"  DESTINATION ${CMAKE_INSTALL_LIBDIR}/gstreamer-1.0)
