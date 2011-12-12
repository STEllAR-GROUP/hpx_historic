# Copyright (c) 2009 Lei Pan
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying 
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

Instructions on how to compile XPI

With root previlige, run cmake with the following options: 
 -DCMAKE_CXX_COMPILER=/usr/bin/g++: define compiler (optional)
 -DCMAKE_PREFIX=/path/xpi: where to install xpi
 -DBOOST_ROOT=/path/boost1-37: the path of boost binary tree (can use "export" in bash) 
 -DHPX_BIN_ROOT=/path/hpx: the path of hpx binary tree
 -DHPX_SRC_ROOT=/path/hpx: the path of hpx source tree


Example: 

% cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_PREFIX=/path/xpi -DBOOST_ROOT=/path/boost1-37 
-DHPX_BIN_ROOT=/path/hpx -DHPX_SRC_ROOT=/path/hpx .
% make
% make install 


