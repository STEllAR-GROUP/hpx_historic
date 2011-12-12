//  Copyright (c) 2007-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/runtime/actions/action_registry.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace actions
{
    bool action_registry::add_action(std::string const& name
      , registry_entry_type const& f)
    {
        registry_type::iterator it = registry_.find(name);
        if (it != registry_.end())
        {
            (*it).second = f;
            return false;
        }
        registry_.insert(registry_type::value_type(name, f));
        return true;
    }
    
    action_registry::registry_entry_type action_registry::get_function(std::string const& name)
    {
        registry_type::iterator it = registry_.find(name);
        return it->second;
    }    
    
}}
