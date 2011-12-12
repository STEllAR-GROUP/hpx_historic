# Copyright (c) 2011 Alex Nagelberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying 
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_hpx_component(action
SOURCES
action_registry.cpp
dynamic_plain_action.cpp
../xpi_action.cpp
ESSENTIAL
)

add_hpx_component(future_action
SOURCES
future_action.cpp
ESSENTIAL
)
