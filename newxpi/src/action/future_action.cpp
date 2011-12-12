/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>
#include <action/future_action.hpp>
#include <action/dynamic_argument.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/components/plain_component_factory.hpp>
#include "../components/xpi_future.hpp"
#include <debug.h>
namespace XPI { namespace action
{

    void action_wrapper(hpx::naming::id_type return_address, xpi::action::dynamic_argument arg)
    {
        hpx::threads::thread_self& self = hpx::threads::get_self();
        hpx::threads::thread_id_type id = self.get_thread_id();
        //hpx::naming::id_type thread_gid=hpx::threads::get_thread_gid(id);
        //std::cout << "future_action::action_wrapper() on " << id << std::endl;
        arg.invoke_action();
        //std::cout << "future_action::action_wrapper() invoked action finished\n"; 
        // TODO: make separate output var for actions

        // Send result back (right now temporarily using same buffer as arg sent)
        WATCH
        XPI::LCO::future_client client(return_address);
        //client.create(return_address);
        
        XPI_Buffer_impl buf=arg.get_buffer();
        client.write(buf);
        WATCH
    }
}}

HPX_REGISTER_COMPONENT_MODULE();

HPX_REGISTER_PLAIN_ACTION_EX(XPI::action::future_action, future_action);
