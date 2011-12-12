/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>
#include <action/dynamic_plain_action.hpp>
#include <action/dynamic_argument.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/components/plain_component_factory.hpp>

namespace xpi { namespace action
{

    int dynamic_action_wrapper(dynamic_argument const& arg)
    {
        return arg.invoke_action();
    }
}}

HPX_REGISTER_COMPONENT_MODULE();

HPX_REGISTER_PLAIN_ACTION_EX(xpi::action::dynamic_plain_action, dynamic_plain_action);
