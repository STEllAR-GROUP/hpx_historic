/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_FUTURE_ACTION_
#define _H_FUTURE_ACTION_

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>

#include <hpx/runtime/actions/plain_action.hpp>
#include "dynamic_argument.hpp"

namespace XPI { namespace action
{
    void action_wrapper(hpx::naming::id_type, xpi::action::dynamic_argument);
    typedef hpx::actions::plain_action2<hpx::naming::id_type, xpi::action::dynamic_argument, action_wrapper> future_action;
}}

#endif
