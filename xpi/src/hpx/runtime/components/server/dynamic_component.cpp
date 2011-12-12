//  Copyright (c) 2007-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/runtime/components/server/dynamic_component.hpp>
#include <xpi.h>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace server
{
    actions::dynamic_argument dynamic_component::dynamic_action_wrapper(
          std::vector<actions::dynamic_argument> const&)
    {
        return actions::dynamic_argument();
    }
}}}
