/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_ACTION_REGISTRY_
#define _H_ACTION_REGISTRY_

#include <xpi.h>

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>
#include <hpx/util/static.hpp>
#include <vector>

#include <hpx/config/warnings_prefix.hpp>

namespace xpi { namespace action
{
    class action_registry
    {
    private:
        typedef XPI_Error(*function_type)(XPI_Action*, XPI_Buffer*);

    public:
        typedef boost::shared_ptr<boost::remove_pointer<function_type>::type> registry_entry_type;
        action_registry() {}
        
    private:
        typedef std::vector<registry_entry_type> registry_type;

        action_registry(const action_registry&);
        action_registry& operator=(const action_registry&);
        
    public:
        int add_action(registry_entry_type const& f);
        registry_entry_type get_function(XPI_Action_impl *action);            
        static action_registry& get_registry()
        {
            hpx::util::static_ <action_registry> singleton;
            return singleton.get();
        }

    private:        
        registry_type registry_;
    };
}}
#endif

