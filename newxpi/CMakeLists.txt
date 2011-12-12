# Copyright (c) 2011 Alex Nagelberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying 
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

project(xpi CXX C)

# set default cmake build type to RelWithDebInfo (None Debug Release RelWithDebInfo MinSizeRel)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(HPX_MAJOR_VERSION 0)
set(HPX_MINOR_VERSION 1)
set(HPX_PATCH_LEVEL 0)
set(HPX_VERSION
"${HPX_MAJOR_VERSION}.${HPX_MINOR_VERSION}.${HPX_PATCH_LEVEL}")
set(HPX_SOVERSION ${HPX_MAJOR_VERSION})

if(NOT DEFINED HPX_ROOT)
  set(HPX_ROOT $ENV{HPX_ROOT})
endif()
set(HPX_INCDIR ${HPX_ROOT}/include)

include(${HPX_ROOT}/share/hpx/cmake/HPX.cmake)

include_directories(${xpi_SOURCE_DIR}/xpi)
include_directories(${xpi_SOURCE_DIR}/internal)

add_subdirectory(src)
