//  Copyright (c) 2007-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_HPX_RUNTIME_ACTIONS_DYNAMIC_PLAIN_ACTION_OCT_28_2009_0907AM)
#define XPI_HPX_RUNTIME_ACTIONS_DYNAMIC_PLAIN_ACTION_OCT_28_2009_0907AM

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>

#include <hpx/runtime/actions/plain_action.hpp>
#include <hpx/runtime/actions/dynamic_argument.hpp>

#include <vector>

#include <xpi.h>
#include <hpx/config/warnings_prefix.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace actions
{
    ///////////////////////////////////////////////////////////////////////////
     int dynamic_action_wrapper(
        dynamic_argument const&);

    ///////////////////////////////////////////////////////////////////////////
    typedef plain_result_action1<
        int
    , dynamic_argument const&
      , dynamic_action_wrapper
    > dynamic_plain_action;
}}

#include <hpx/config/warnings_suffix.hpp>

#endif
