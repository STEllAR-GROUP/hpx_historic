/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <xpi_action.h>
#include <action/dynamic_plain_action.hpp>
#include <hpx/hpx.hpp>

struct XPI_Future_impl
{
    hpx::lcos::eager_future<xpi::action::dynamic_plain_action> future;
};

XPI_API_EXPORT XPI_Error XPI_Eager_Future_create(XPI_Future *future, XPI_Action *action, XPI_Buffer *arg, XPI_Gid *gid)
{
    xpi::action::dynamic_argument dyn_arg(action, arg);

    hpx::naming::gid_type hpx_gid(gid->id_msb_, gid->id_lsb_);

    future->impl=new XPI_Future_impl;
    future->impl->future=hpx::lcos::eager_future<xpi::action::dynamic_plain_action>(hpx_gid, dyn_arg);
    
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Eager_Future_wait(XPI_Future *future)
{
    return future->impl->future.get();
}

XPI_API_EXPORT XPI_Error XPI_Eager_Future_destroy(XPI_Future *future)
{
    delete future->impl;

    return XPI_SUCCESS;
}
