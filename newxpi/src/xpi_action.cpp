/*
 * Copyright (c) 2011 Alex Nagelberg
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
#include <action/action_registry.hpp>

#include <xpi_action.h>
#include <xpi_action.hpp>

XPI_API_EXPORT XPI_Error XPI_Action_create(const char *name, XPI_Action *action)
{
    // Split key in to procedure and function
    std::string key(name);

    // TODO: fix warning here, needs to determine if string is sane
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
    typedef XPI_Error(*function_type)(XPI_Action*, XPI_Buffer*);
    typedef boost::function<void(function_type)> deleter_type;
    std::pair<function_type, deleter_type> pair=d.get<function_type,deleter_type>(function);
    boost::shared_ptr<boost::remove_pointer<function_type>::type> s(pair.first, pair.second);

    action->impl=new XPI_Action_impl;
    action->impl->name=name;
    action->impl->registry_index=xpi::action::action_registry::get_registry().add_action(s);
   
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Action_destroy(XPI_Action *action)
{
    delete action->impl;
    return XPI_SUCCESS;
}
