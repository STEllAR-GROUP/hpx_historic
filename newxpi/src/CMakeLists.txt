# Copyright (c) 2011 Alex Nagelberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying 
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_subdirectory(action)
add_subdirectory(components)

add_library(xpi SHARED
xpi_globals.cpp
xpi_buffer.cpp
)

link_directories("${CMAKE_SOURCE_DIR}/src/components")
link_directories("${CMAKE_SOURCE_DIR}/src/action")
add_hpx_executable(test1 SOURCES test.c DEPENDENCIES xpi xpi_future_component action_component future_action_component ESSENTIAL)

add_hpx_library(foo SOURCES foo.c DEPENDENCIES xpi xpi_future_component
action_component future_action_component ESSENTIAL)
