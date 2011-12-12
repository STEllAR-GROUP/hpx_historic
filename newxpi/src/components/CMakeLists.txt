# Copyright (c) 2011 Alex Nagelberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying 
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_hpx_component(LCO
MODULE LCO
SOURCES
LCO.cpp

HEADERS
LCO.hpp 
ESSENTIAL)

add_hpx_component(xpi_mutex
MODULE xpi_mutex
SOURCES
xpi_mutex.cpp
HEADERS
xpi_mutex.hpp
LCO.hpp
ESSENTIAL)

add_hpx_component(xpi_future
MODULE xpi_future
SOURCES
xpi_future.cpp
#../xpi_lco.cpp

HEADERS
xpi_future.hpp
LCO.hpp
ESSENTIAL)

add_hpx_component(work_queue
MODULE work_queue

SOURCES
work_queue.cpp

HEADERS
work_queue.hpp work_queue_impl.hpp

ESSENTIAL)

