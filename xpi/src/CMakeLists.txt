# Copyright (c) 2009 Alex Nagelberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

###############################################################################
# Files extending HPX 
set (xpi_hpx_actions_HEADERS
    ${xpi_SOURCE_DIR}/internal/hpx/runtime/actions/action_registry.hpp
    ${xpi_SOURCE_DIR}/internal/hpx/runtime/actions/dynamic_argument.hpp
    ${xpi_SOURCE_DIR}/internal/hpx/runtime/actions/dynamic_plain_action.hpp)
source_group("Header Files\\HPX\\Runtime\\Actions" 
    FILES ${xpi_hpx_actions_HEADERS})

set (xpi_hpx_components_HEADERS
    ${xpi_SOURCE_DIR}/internal/hpx/runtime/components/server/dynamic_component.hpp
    )

source_group("Header Files\\HPX\\Runtime\\Components" 
    FILES ${xpi_hpx_components_HEADERS})

set (xpi_multicaster_component_HEADERS
    ${xpi_SOURCE_DIR}/internal/xpi/components/multicaster.hpp
    ${xpi_SOURCE_DIR}/internal/xpi/components/multicaster_impl.hpp)

set (xpi_mutex_component_HEADERS
    ${xpi_SOURCE_DIR}/internal/xpi/components/mutex.hpp
    ${xpi_SOURCE_DIR}/internal/xpi/components/mutex_impl.hpp)

set (xpi_future_component_HEADERS
    ${xpi_SOURCE_DIR}/internal/xpi/components/future.hpp
    ${xpi_SOURCE_DIR}/internal/xpi/components/future_impl.hpp)


set (xpi_types_HEADERS
    ${xpi_SOURCE_DIR}/internal/xpi/types/gid_list.hpp
    ${xpi_SOURCE_DIR}/internal/xpi/types/xpi_action.hpp
    ${xpi_SOURCE_DIR}/internal/xpi/types/xpi_thread.hpp
    )

set (xpi_internal_HEADERS
    ${xpi_multicaster_component_HEADERS}
    ${xpi_types_HEADERS})

set (xpi_hpx_HEADERS
    ${xpi_hpx_actions_HEADERS}
    ${xpi_hpx_components_HEADERS})

###############################################################################
# source files
set (xpi_hpx_actions_SOURCES
    ${xpi_SOURCE_DIR}/src/hpx/runtime/actions/action_registry.cpp
    ${xpi_SOURCE_DIR}/src/hpx/runtime/actions/dynamic_plain_action.cpp)
source_group("Source Files\\HPX\\Runtime\\Actions" 
    FILES ${xpi_hpx_actions_SOURCES})

set (xpi_hpx_components_SOURCES
    ${xpi_SOURCE_DIR}/src/hpx/runtime/components/server/dynamic_component.cpp)
    
source_group("Source Files\\HPX\\Runtime\\Components\\Server" 
    FILES ${xpi_hpx_components_SOURCES})

set (xpi_multicaster_component_SOURCES
    ${xpi_SOURCE_DIR}/src/components/multicaster.cpp
    ${xpi_SOURCE_DIR}/src/xpi_global.cpp)

set (xpi_mutex_component_SOURCES
    ${xpi_SOURCE_DIR}/src/components/mutex.cpp
    )

set (xpi_future_component_SOURCES
	${xpi_SOURCE_DIR}/src/components/future.cpp
	)

set (xpi_hpx_SOURCES
    ${xpi_hpx_actions_SOURCES} 
    ${xpi_hpx_components_SOURCES}   
    )

###############################################################################
# create XPI library target
add_definitions(-DHPX_COMPONENT_NAME=multicaster)
add_definitions(-DHPX_COMPONENT_LIB_NAME=libhpx_component_multicaster)
add_definitions(-DHPX_COMPONENT_EXPORTS)
add_definitions(-DXPI_EXPORTS)

add_library(hpx_component_multicaster SHARED
    ${xpi_SOURCE_DIR}/src/hpx/hpx_module.cpp
    ${xpi_multicaster_component_SOURCES}
    ${xpi_hpx_SOURCES}
    ${xpi_hpx_HEADERS}
    ${xpi_internal_HEADERS})

set_target_properties(hpx_component_multicaster PROPERTIES
    VERSION ${XPI_VERSION}      # create *nix style library versions + symbolic links
    SOVERSION ${XPI_SOVERSION}
    CLEAN_DIRECT_OUTPUT 1       # allow creating static and shared libs without conflicts
    OUTPUT_NAME hpx_component_multicaster)

add_definitions(-DHPX_COMPONENT_NAME=mutex)
add_definitions(-DHPX_COMPONENT_LIB_NAME=libhpx_component_mutex)
add_definitions(-DHPX_COMPONENT_EXPORTS)
add_definitions(-DXPI_EXPORTS)

add_library(hpx_component_mutex SHARED
    ${xpi_mutex_component_SOURCES}
    ${xpi_mutex_component_HEADERS}
    ${xpi_hpx_HEADERS}
    ${xpi_internal_HEADERS}
    )

set_target_properties(hpx_component_mutex PROPERTIES
    VERSION ${XPI_VERSION}      # create *nix style library versions + symbolic links
    SOVERSION ${XPI_SOVERSION}
    CLEAN_DIRECT_OUTPUT 1       # allow creating static and shared libs without conflicts
    OUTPUT_NAME hpx_component_mutex)

add_definitions(-DHPX_COMPONENT_NAME=future)
add_definitions(-DHPX_COMPONENT_LIB_NAME=libhpx_component_future)
add_definitions(-DHPX_COMPONENT_EXPORTS)
add_definitions(-DXPI_EXPORTS)

add_library(hpx_component_future SHARED
    ${xpi_future_component_SOURCES}
    ${xpi_future_component_HEADERS}    
    ${xpi_hpx_HEADERS}
    ${xpi_internal_HEADERS}
    )
target_link_libraries(hpx_component_future xpi)

set_target_properties(hpx_component_future PROPERTIES
    VERSION ${XPI_VERSION}      # create *nix style library versions + symbolic links
    SOVERSION ${XPI_SOVERSION}
    CLEAN_DIRECT_OUTPUT 1       # allow creating static and shared libs without conflicts
    OUTPUT_NAME hpx_component_future)

add_definitions(-DHPX_COMPONENT_NAME=xpi)
add_definitions(-DHPX_COMPONENT_EXPORTS)
add_definitions(-DXPI_EXPORTS)

add_library(xpi SHARED 
    xpi_global.cpp xpi_threads.cpp xpi_parcels.cpp xpi_lcos.cpp
    ${xpi_SOURCE_DIR}/src/hpx/hpx_module.cpp
    ${xpi_hpx_SOURCES} 
    ${xpi_hpx_HEADERS}
    ${xpi_internal_HEADERS})

add_library(foo SHARED foo.c)
target_link_libraries(foo xpi hpx_component_future)
#)

# set properties of generated shared library
set_target_properties(xpi PROPERTIES
    VERSION ${XPI_VERSION}      # create *nix style library versions + symbolic links
    SOVERSION ${XPI_SOVERSION}
    CLEAN_DIRECT_OUTPUT 1       # allow creating static and shared libs without conflicts
    OUTPUT_NAME xpi)
target_link_libraries(xpi hpx_component_multicaster)

add_executable(test1 test.cpp)
target_link_libraries(test1 xpi)

add_executable(thread_perf_test thread_perf_test.cpp)
target_link_libraries(thread_perf_test xpi)
