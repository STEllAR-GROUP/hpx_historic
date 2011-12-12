//  Copyright (c) 2007-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_HPX_RUNTIME_ACTIONS_ACTION_REGISTRY_NOV_11_2009_1017AM)
#define XPI_HPX_RUNTIME_ACTIONS_ACTION_REGISTRY_NOV_11_2009_1017AM

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>
#include <hpx/util/static.hpp>
#include <xpi.h>

#include <hpx/config/warnings_prefix.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace actions
{
    
    ///////////////////////////////////////////////////////////////////////////
    class action_registry
    {
    private:
        
        typedef XPI_Error(*function_type)(XPI_Action*, XPI_Struct*);


    public:
        typedef boost::shared_ptr<boost::remove_pointer<function_type>::type> registry_entry_type;
        action_registry() {}
        
    private:
        typedef std::map<std::string, registry_entry_type> registry_type;

        action_registry(const action_registry&);
        action_registry& operator=(const action_registry&);
        
    public:
        bool add_action(std::string const& name, registry_entry_type const& f);
        registry_entry_type get_function(std::string const& name);            
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
