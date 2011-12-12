/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_DYNAMIC_PLAIN_ACTION_
#define _H_DYNAMIC_PLAIN_ACTION_

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>

#include <hpx/runtime/actions/plain_action.hpp>
#include "dynamic_argument.hpp"

namespace xpi { namespace action
{
    int dynamic_action_wrapper(dynamic_argument const&);
    typedef hpx::actions::plain_result_action1<int, dynamic_argument const&, dynamic_action_wrapper> dynamic_plain_action;
}}

#endif
