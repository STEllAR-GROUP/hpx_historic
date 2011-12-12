//  Copyright (c) 2010 Alex Nagelberg
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_COMPONENTS_MUTEX_IMPL_APR_28_2010_1113AM)
#define XPI_COMPONENTS_MUTEX_IMPL_APR_28_2010_1113AM

#include <xpi/components/mutex.hpp>
#include <hpx/lcos/mutex.hpp>
#include <xpi/components/multicaster.hpp>
struct XPI_LCO_impl
{
    xpi::components::mutex::client *mutex_lco;
};

void xpi::components::mutex::server::reg(xpi::types::gid_list arg0)
{
    XPI_Gid gid=arg0.get_gids()[0];
    hpx::lcos::mutex::scoped_lock l(mtx);
    if (lock.id_msb_==0 && lock.id_lsb_==0) // if lock's free, grab lock
        lock=gid;
    else // otherwise go in to queue
        queue.push_back(gid);
}

void xpi::components::mutex::server::wait(xpi::types::gid_list arg0)
{
    //wait until lock is current gid, send wakeup (apply to gid)    
    XPI_Gid gid=arg0.get_gids()[0];
    while (lock.id_msb_!=gid.id_msb_ && lock.id_lsb_!=gid.id_lsb_);
    
    //send wakeup
    hpx::naming::id_type hpx_gid(gid.id_msb_, gid.id_lsb_, hpx::naming::id_type::unmanaged);
    hpx::applier::apply<hpx::lcos::base_lco::set_event_action>(hpx_gid);
}

void xpi::components::mutex::server::unreg(xpi::types::gid_list arg0)
{
    //atomically give away lock to next on list
    hpx::lcos::mutex::scoped_lock l(mtx);
    if (!queue.empty())
    {
        lock=queue.front();
        queue.pop_front();
    }
    else
    {
        lock.id_msb_=0;
        lock.id_lsb_=0;
    }
}

XPI_Error XPI_Mutex_create(XPI_LCO *lco)
{
    hpx::applier::applier& appl = hpx::applier::get_applier();
    std::vector<hpx::naming::gid_type> prefixes;
    hpx::naming::gid_type gid = appl.get_runtime_support_raw_gid();
    lco->impl=new XPI_LCO_impl;
    xpi::components::mutex::client *cli=new xpi::components::mutex::client;
    cli->create(gid);
    lco->impl->mutex_lco=cli;
    return XPI_SUCCESS;
}

XPI_Error XPI_Mutex_destroy(XPI_LCO *lco)
{
    delete lco->impl->mutex_lco;
    delete lco->impl;
    lco->impl=0;
    return XPI_SUCCESS;
}

XPI_Error XPI_Mutex_register(XPI_LCO *lco)
{
    hpx::threads::thread_self& self = hpx::threads::get_self();
    hpx::threads::thread_id_type id = self.get_thread_id();
    hpx::naming::id_type thread_gid=hpx::threads::get_thread_gid(id);
    XPI_Gid *xpi_thread_gid=new XPI_Gid;
    xpi_thread_gid->id_msb_=thread_gid.get_msb();
    xpi_thread_gid->id_lsb_=thread_gid.get_lsb();
    xpi::types::gid_list gids(xpi_thread_gid, 1);
    lco->impl->mutex_lco->reg(gids);
    return XPI_SUCCESS;
}

XPI_Error XPI_Mutex_unregister(XPI_LCO *lco)
{
    hpx::threads::thread_self& self = hpx::threads::get_self();
    hpx::threads::thread_id_type id = self.get_thread_id();
    hpx::naming::id_type thread_gid=hpx::threads::get_thread_gid(id);
    XPI_Gid *xpi_thread_gid=new XPI_Gid;
    xpi_thread_gid->id_msb_=thread_gid.get_msb();
    xpi_thread_gid->id_lsb_=thread_gid.get_lsb();
    xpi::types::gid_list gids(xpi_thread_gid, 1);
    lco->impl->mutex_lco->unreg(gids);
    return XPI_SUCCESS;
}

XPI_Error XPI_Mutex_wait(XPI_LCO *lco)
{
    hpx::threads::thread_self& self = hpx::threads::get_self();
    hpx::threads::thread_id_type id = self.get_thread_id();
    hpx::naming::id_type thread_gid=hpx::threads::get_thread_gid(id);
    XPI_Gid *xpi_thread_gid=new XPI_Gid;
    xpi_thread_gid->id_msb_=thread_gid.get_msb();
    xpi_thread_gid->id_lsb_=thread_gid.get_lsb();
    xpi::types::gid_list gids(xpi_thread_gid, 1);   
    lco->impl->mutex_lco->wait(gids);
    self.yield(hpx::threads::suspended);
    return XPI_SUCCESS;
}

#endif
