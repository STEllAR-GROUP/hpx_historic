/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi_action.hpp>
#include <action/action_registry.hpp>

namespace xpi { namespace action
{
    int action_registry::add_action(registry_entry_type const& f)
    {
        registry_.push_back(f);
        return registry_.size()-1;
    }

    action_registry::registry_entry_type action_registry::get_function(XPI_Action_impl *action)
    {
        return registry_[action->registry_index];
    }

}}
