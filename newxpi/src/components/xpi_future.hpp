/*
  Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef _HPP_XPI_FUTURE_
#define _HPP_XPI_FUTURE_

#include <iostream>
#include <xpi.h>
#include <xpi_buffer.hpp>
#include <hpx/hpx.hpp>
#include "LCO.hpp"
#include "action/dynamic_argument.hpp"
#include "action/future_action.hpp"
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/runtime/components/component_factory.hpp>
#include <hpx/runtime/components/client_base.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/components/stubs/stub_base.hpp>

#include <debug.h>

namespace XPI { namespace LCO 
{
    class future_server : base_lco, hpx::components::detail::managed_component_base<future_server>
    {
    private:
        //XPI_Buffer receptor;
        //hpx::naming::id_type thread_waiting;
        hpx::threads::thread_id_type thread_waiting;
        bool data_arrived;

    public:
        XPI_Buffer receptor;
        typedef base_lco base_type_holder;
        // disambiguate base classes
        typedef hpx::components::detail::managed_component_base<future_server> base_type;
        using base_type::finalize;
        typedef base_type::wrapping_type wrapping_type;

        static hpx::components::component_type get_component_type()
        {   
            return hpx::components::get_component_type<future_server>();
        }
        static void set_component_type(hpx::components::component_type type)
        {   
            hpx::components::set_component_type<future_server>(type);
        }

        void init()
        {
        }

        void reg(hpx::naming::id_type loopback_gid, hpx::naming::id_type dest_gid, xpi::action::dynamic_argument arg, bool isEager)
        {
            data_arrived=false;
            if (isEager)
            {
                hpx::applier::apply<XPI::action::future_action>(dest_gid, loopback_gid, arg);//thread_gid, arg);
                //std::cout << "server::req() on " << id << std::endl;
            }
        }

        void write(XPI_Buffer_impl data)
        {
            WATCH
            
            size_t len=data.get_size();
            XPI_Buffer_create(&receptor, len, 0);
            memcpy(receptor.buf, data.get_buffer(), len);
            //receptor=data;
            /*while (gid_waiting==NULL);
            //{
                std::cout << "waking up " << gid_waiting << std::endl;
                hpx::applier::apply<hpx::lcos::base_lco::set_event_action>(gid_waiting); // wake up thread
                // set gid_waiting to null
            //}
      
            //else
            //    std::cout << "already got result before something waited\n";
            */
            WATCH
            while (thread_waiting==NULL);
            //{
                WATCH
                while (hpx::threads::get_thread_state(thread_waiting)!=hpx::threads::suspended); // wait on pending thread suspension
                WATCH
                set_thread_state(thread_waiting, hpx::threads::pending);
            //}
                //hpx::applier::apply<hpx::lcos::base_lco::set_event_action>(thread_waiting);
            /*hpx::threads::thread_self& self = hpx::threads::get_self();
            hpx::threads::thread_id_type id = self.get_thread_id();
            //hpx::naming::id_type thread_gid=hpx::threads::get_thread_gid(id);
            */
            WATCH
            data_arrived=true;
        }

        XPI_Buffer_impl wait(hpx::naming::id_type gid)
        {
            hpx::threads::thread_self& self = hpx::threads::get_self();
            hpx::threads::thread_id_type id = self.get_thread_id();
           
            // put current thread to sleep
            //std::cout << "server::wait() on " << id << std::endl;
            WATCH
            if (!data_arrived)
            {
                //thread_waiting=hpx::threads::get_thread_gid(id);
                thread_waiting=id;
                WATCH
                self.yield(hpx::threads::suspended);
            }
            WATCH
            return *receptor.impl;            
        }
        
        enum 
        {
            init_func_code,
            write_func_code,
            wait_func_code,
            reg_func_code,
        } func_codes;

        typedef hpx::actions::action4<future_server, reg_func_code, hpx::naming::id_type, hpx::naming::id_type, xpi::action::dynamic_argument, bool, &future_server::reg> reg_action;
        typedef hpx::actions::action0<future_server, init_func_code, &future_server::init> init_action;
        typedef hpx::actions::action1<future_server, write_func_code, XPI_Buffer_impl, &future_server::write> write_action;
        typedef hpx::actions::result_action1<future_server, XPI_Buffer_impl, wait_func_code, hpx::naming::id_type, &future_server::wait> wait_action;
    };


    class future_stub : public hpx::components::stubs::stub_base<future_server>
    {
    public:

        static void init(hpx::naming::id_type gid)
        {  
            hpx::applier::apply<future_server::init_action>(gid);
        }

        static void write(hpx::naming::id_type gid, XPI_Buffer_impl arg0)
        {  
            hpx::applier::apply<future_server::write_action>(gid, arg0);
        }

        static XPI_Buffer_impl wait(hpx::naming::id_type gid, hpx::naming::id_type arg0)
        {  
            return hpx::lcos::eager_future<future_server::wait_action>(gid, arg0).get();
        }

        static void reg(hpx::naming::id_type gid, hpx::naming::id_type arg0, hpx::naming::id_type arg1, xpi::action::dynamic_argument arg2, bool arg3)
        {
            hpx::applier::apply<future_server::reg_action>(gid, arg0, arg1, arg2, arg3);
        }
    };

    class future_client : public hpx::components::client_base<future_client, future_stub>
    {
        typedef hpx::components::client_base<future_client, future_stub> base_type;
    public:
        future_client(hpx::naming::id_type gid) : base_type(gid) {}
        future_client() : base_type() {}

        void init()
        {
            BOOST_ASSERT(gid_);
            this->base_type::init(gid_);
        }

        void write(XPI_Buffer_impl arg0)
        {
            BOOST_ASSERT(gid_);
            this->base_type::write(gid_, arg0);
        }
   
        XPI_Buffer_impl wait(hpx::naming::id_type arg0)
        {
            BOOST_ASSERT(gid_);
            return this->base_type::wait(gid_, arg0);
        }
       
        void reg(hpx::naming::id_type arg0, hpx::naming::id_type arg1, xpi::action::dynamic_argument arg2, bool arg3)
        {
            BOOST_ASSERT(gid_);
            this->base_type::reg(gid_, arg0, arg1, arg2, arg3);
        }
    };
} } 

#endif
