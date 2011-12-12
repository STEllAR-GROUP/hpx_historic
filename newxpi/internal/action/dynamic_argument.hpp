/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_DYNAMIC_ARGUMENT_
#define _H_DYNAMIC_ARGUMENT_

#include <xpi.h>
#include "action_registry.hpp"
#include "xpi_buffer.hpp"
#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>
#include <hpx/config/warnings_prefix.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>

#define XPI_DYNAMIC_ARGUMENT_VERSION 0x10

namespace xpi { namespace action
{
    class dynamic_argument_impl
    {
    private:
        XPI_Action *action;
        XPI_Buffer *arg;
    public:
        XPI_Buffer_impl get_buffer()
        {
            return *arg->impl;
        }

        dynamic_argument_impl(XPI_Action *_action, XPI_Buffer *_arg)
        {
            action=_action;
            arg=_arg;
        }

        dynamic_argument_impl() { }

        XPI_Error invoke_action()
        {
            return action_registry::get_registry().get_function(action->impl).get()(action, arg);
        }
    };

    class dynamic_argument
    {
    public:
        boost::shared_ptr<dynamic_argument_impl> ptr;
        dynamic_argument() : ptr(new dynamic_argument_impl()) { }
        dynamic_argument(const dynamic_argument &da) : ptr(da.ptr) {  }
        dynamic_argument(XPI_Action *_action, XPI_Buffer *_arg) : ptr(new dynamic_argument_impl(_action, _arg)) { }
        XPI_Error invoke_action() const
        {
            return ptr->invoke_action();
        }

        XPI_Buffer_impl get_buffer()
        {
            return ptr->get_buffer();
        }
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive &ar, const unsigned int version) const
        {   
        //    ar & *ptr;
        }

        template<class Archive>
        void load(Archive &ar, const unsigned int version)
        {   
        //    ar & *ptr;
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
}}

BOOST_CLASS_VERSION(xpi::action::dynamic_argument, XPI_DYNAMIC_ARGUMENT_VERSION)
BOOST_CLASS_TRACKING(xpi::action::dynamic_argument, boost::serialization::track_never)

#endif

