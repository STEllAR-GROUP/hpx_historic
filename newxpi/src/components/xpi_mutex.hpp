/*
  Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef _HPP_XPI_MUTEX_
#define _HPP_XPI_MUTEX_

#include <iostream>
#include <xpi.h>
#include <xpi_buffer.hpp>
#include <hpx/hpx.hpp>
#include "LCO.hpp"
#include "action/dynamic_argument.hpp"
#include <hpx/runtime/components/component_factory.hpp>
#include <hpx/runtime/components/client_base.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/components/stubs/stub_base.hpp>

#include <queue>

namespace XPI { namespace LCO 
{
    class mutex_server : base_lco, hpx::components::detail::managed_component_base<mutex_server>
    {
    private:
        XPI_Buffer receptor;
        //hpx::naming::id_type gid_waiting;
        //hpx::threads::thread_id_type thread_waiting;
        //std::queue<hpx::threads::thread_id_type> threads_waiting;
        hpx::lcos::mutex lock;
        
    public:
        typedef base_lco base_type_holder;
        // disambiguate base classes
        typedef hpx::components::detail::managed_component_base<mutex_server> base_type;
        using base_type::finalize;
        typedef base_type::wrapping_type wrapping_type;

        static hpx::components::component_type get_component_type()
        {   
            return hpx::components::get_component_type<mutex_server>();
        }
        static void set_component_type(hpx::components::component_type type)
        {   
            hpx::components::set_component_type<mutex_server>(type);
        }

        void init()
        {
           
        }

        void write(XPI_Buffer_impl data)
        {
            // release lock
            lock.unlock(); 
        }

        XPI_Buffer_impl wait(hpx::naming::id_type gid)
        {
            hpx::threads::thread_self& self = hpx::threads::get_self();
            hpx::threads::thread_id_type id = self.get_thread_id();
            
            // put current thread to sleep
            std::cout << "server::wait() on " << id << std::endl;
            self.yield(hpx::threads::suspended);

            // wait for lock, then wake up thread
            lock.lock();
            set_thread_state(id, hpx::threads::pending);
            
            return *receptor.impl;            
        }
        
        enum 
        {
            init_func_code,
            write_func_code,
            wait_func_code,
            //reg_func_code,
        } func_codes;

        //typedef hpx::actions::action3<future_server, reg_func_code, hpx::naming::id_type, xpi::action::dynamic_argument, bool, &future_server::reg> reg_action;
        typedef hpx::actions::action0<mutex_server, init_func_code, &mutex_server::init> init_action;
        typedef hpx::actions::action1<mutex_server, write_func_code, XPI_Buffer_impl, &mutex_server::write> write_action;
        typedef hpx::actions::result_action1<mutex_server, XPI_Buffer_impl, wait_func_code, hpx::naming::id_type, &mutex_server::wait> wait_action;
    };


    class mutex_stub : public hpx::components::stubs::stub_base<mutex_server>
    {
    public:

        static void init(hpx::naming::id_type gid)
        {  
            hpx::applier::apply<mutex_server::init_action>(gid);
        }

        static void write(hpx::naming::id_type gid, XPI_Buffer_impl arg0)
        {  
            hpx::applier::apply<mutex_server::write_action>(gid, arg0);
        }

        static XPI_Buffer_impl wait(hpx::naming::id_type gid, hpx::naming::id_type arg0)
        {  
            return hpx::lcos::eager_future<mutex_server::wait_action>(gid, arg0).get();
        }

        /*static void reg(hpx::naming::id_type gid, hpx::naming::id_type arg0, xpi::action::dynamic_argument arg1, bool arg2)
        {
            hpx::applier::apply<future_server::reg_action>(gid, arg0, arg1, arg2);
        }*/
    };

    class mutex_client : public hpx::components::client_base<mutex_client, mutex_stub>
    {
        typedef hpx::components::client_base<mutex_client, mutex_stub> base_type;
    public:
        mutex_client(hpx::naming::id_type gid) : base_type(gid) {}
        mutex_client() : base_type() {}

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
       
        /*void reg(hpx::naming::id_type arg0, xpi::action::dynamic_argument arg1, bool arg2)
        {
            BOOST_ASSERT(gid_);
            this->base_type::reg(gid_, arg0, arg1, arg2);
        }*/
    };
} } 

#endif
