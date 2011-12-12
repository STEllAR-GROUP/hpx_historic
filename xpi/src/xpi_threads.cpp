/*
 * Copyright (c) 2009 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <iostream>
#include <hpx/hpx.hpp>
#include <boost/plugin.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <hpx/runtime/actions/action_registry.hpp>
#include <xpi/types/xpi_action.hpp>
#include <xpi/types/xpi_thread.hpp>

XPI_Error XPI_Action_create(char *name, size_t arg_count, size_t type_size, XPI_Type *types, XPI_Action *action)
{
    // Split key in to procedure and function
    std::string key(name);
    
    int dot=key.find('.');
    if (dot==std::string::npos)
    {
        std::cerr << "Invalid action name format given: " << name << "\n";
        return XPI_ERROR;
    }
    
    std::string procedure=key.substr(0, dot);
    std::string function=key.substr(dot+1);
    
    // parse ini, get path from key
    hpx::util::section const& ini = hpx::get_runtime().get_config();
    std::string library_path;
    try
    {
        hpx::util::section const *sec = ini.get_section(procedure);        
        library_path = sec->get_entry("path");
    }
    catch (...)
    {
        std::cerr << "Error parsing path from configuration\n";
        return XPI_ERROR;
    }
    
    boost::plugin::dll d(library_path);
    
    // get function and store in table with action
    typedef XPI_Error(*function_type)(XPI_Action*, XPI_Struct*);
    typedef boost::function<void(function_type)> deleter_type;
    std::pair<function_type, deleter_type> pair=d.get<function_type,deleter_type>(function);
    boost::shared_ptr<boost::remove_pointer<function_type>::type> s(pair.first, pair.second);
    
    action->impl=new XPI_Action_impl;
    
    action->impl->name=name;
    action->impl->arg_count=arg_count;
    action->impl->type_size=type_size;
    action->impl->arg_types=types;

    
    
    hpx::actions::action_registry::get_registry().add_action(name,s);
    
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Action_delete(XPI_Action *action)
{
    delete action->impl;
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Thread_terminate(XPI_Thread *thread)
{
    thread->impl->state=XPI_THREADSTATE_DEPLETED;
    //thread->impl->thread.yield(hpx::threads::suspended);
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Thread_suspend(XPI_Thread *thread)
{
    thread->impl->state=XPI_THREADSTATE_SUSPENDED;
    thread->impl->thread.yield(hpx::threads::suspended);    
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Thread_self(XPI_Thread *thread)
{
    thread->impl=new XPI_Thread_impl(hpx::threads::get_self(), XPI_THREADSTATE_EXECUTING);
//     /*thread->impl->thread =*/ hpx::threads::get_self();
//     thread->impl->state=XPI_THREADSTATE_EXECUTING; // if something is asking about itself, it's running I assume
    return XPI_SUCCESS;
}
