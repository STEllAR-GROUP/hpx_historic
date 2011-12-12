# Copyright (c) 2009 Alex Nagelberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#add_executable(test test.c)
#target_link_libraries(test xpi)

# Make sure to symlink/copy CuTest.c and CuTest.h in to this directory
add_executable(global_unit global_unit.c CuTest.c)
target_link_libraries(global_unit xpi)

add_test(GlobalUnitTest global_unit "-r")

