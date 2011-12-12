//  Copyright (c) 2010 Alex Nagelberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_COMPONENTS_FUTURE_IMPL_JUL_15_2010_0213PM)
#define XPI_COMPONENTS_FUTURE_IMPL_JUL_15_2010_0213PM

#include <xpi/components/future.hpp>
#include <hpx/runtime/actions/dynamic_plain_action.hpp>
#include <hpx/lcos/lazy_future.hpp>
#include <hpx/lcos/eager_future.hpp>

struct XPI_LCO_impl
{
    xpi::components::future::client *future_lco;
};

void xpi::components::future::server::reg(hpx::actions::dynamic_argument arg, bool eager)
{
    mtx.lock();
    isEager=eager;
    if (eager)
    {
        hpx::applier::applier& appl = hpx::applier::get_applier();
        std::vector<hpx::naming::gid_type> prefixes;
        hpx::naming::gid_type gid = appl.get_runtime_support_raw_gid();

        hpx::lcos::eager_future<hpx::actions::dynamic_plain_action> xpi_future(gid, arg);
        result=xpi_future.get();
    }
    else
        _arg=arg;
    mtx.unlock();
}

void xpi::components::future::server::wait(xpi::types::gid_list gids)
{
    mtx.lock();
    if (!isEager)
    {
        hpx::applier::applier& appl = hpx::applier::get_applier();
        std::vector<hpx::naming::gid_type> prefixes;
        hpx::naming::gid_type gid = appl.get_runtime_support_raw_gid();

        hpx::lcos::eager_future<hpx::actions::dynamic_plain_action> xpi_future(gid, _arg);
        result=xpi_future.get();
    }
    XPI_Gid gid=gids.get_gids()[0];

    //send wakeup
    hpx::naming::id_type hpx_gid(gid.id_msb_, gid.id_lsb_, hpx::naming::id_type::unmanaged);
    hpx::applier::apply<hpx::lcos::base_lco::set_event_action>(hpx_gid);

    // TODO: this needs to be replaced badly. I'm just using a wrapper temporarily to put the value in to a XPI_Arg. Actions need to just always return XPI_Args instead.
    //XPI_Struct *arg=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct));
    //arg[0].i8=result;
    //hpx::actions::dynamic_argument hpx_arg(NULL, arg, 0, NULL, false);
    mtx.unlock();
    //return hpx_arg;
}

void xpi::components::future::server::unreg()
{
    // only really applies if eager. remove all action data.
}

XPI_Error XPI_Future_create(XPI_LCO *lco)
{
    hpx::applier::applier& appl = hpx::applier::get_applier();
    std::vector<hpx::naming::gid_type> prefixes;
    hpx::naming::gid_type gid = appl.get_runtime_support_raw_gid();
    lco->impl=new XPI_LCO_impl;
    xpi::components::future::client *cli=new xpi::components::future::client;
    cli->create(gid);
    lco->impl->future_lco=cli;
    return XPI_SUCCESS;
}

XPI_Error XPI_Future_register(XPI_LCO *lco, XPI_Action *action, XPI_Struct *arg, bool eager)
{

    hpx::actions::dynamic_argument hpx_arg(action->impl, arg, 0, NULL, false); // TODO: need to fix this memory allocation
    lco->impl->future_lco->reg(hpx_arg, eager);

    return XPI_SUCCESS;
}

XPI_Error XPI_Future_unregister(XPI_LCO *lco)
{
    lco->impl->future_lco->unreg();
    delete lco->impl->future_lco;
    delete lco->impl;
    return XPI_SUCCESS;
}

XPI_Error XPI_Future_wait(XPI_LCO *lco, void *result)
{
    hpx::threads::thread_self& self = hpx::threads::get_self();
    hpx::threads::thread_id_type id = self.get_thread_id();
    hpx::naming::id_type thread_gid=hpx::threads::get_thread_gid(id);
    XPI_Gid *xpi_thread_gid=new XPI_Gid;
    xpi_thread_gid->id_msb_=thread_gid.get_msb();
    xpi_thread_gid->id_lsb_=thread_gid.get_lsb();
    xpi::types::gid_list gids(xpi_thread_gid, 1);

    /*hpx::actions::dynamic_argument ret=*/lco->impl->future_lco->wait(gids);

    self.yield(hpx::threads::suspended);

    // get result
    return XPI_SUCCESS;
}

#endif
