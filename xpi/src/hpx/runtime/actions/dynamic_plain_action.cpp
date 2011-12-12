//  Copyright (c) 2007-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/runtime/actions/dynamic_plain_action.hpp>
#include <xpi.h>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace actions
{
    int dynamic_action_wrapper(
          dynamic_argument const& arg)
    {        
        return arg.invoke_action();
        //return 0;
    }
}}
