//  Copyright (c) 2010 Alex Nagelberg
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_COMPONENTS_MULTICASTER_IMPL_MAR_5_2010_1113AM)
#define XPI_COMPONENTS_MULTICASTER_IMPL_MAR_5_2010_1113AM

#include <xpi/components/multicaster.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/actions/dynamic_plain_action.hpp>
void xpi::components::multicaster::server::init(xpi::types::gid_list _gids, hpx::actions::dynamic_argument _args)
{
    gids=_gids;
    args=_args;
}

void xpi::components::multicaster::server::fire()
{
    size_t cnt=gids.size();
    XPI_Gid *list=gids.get_gids();
    size_t i;
    for (i=0; i<cnt; ++i)
    {
        hpx::naming::id_type gid(list[i].id_msb_, list[i].id_lsb_, hpx::naming::id_type::unmanaged);
        hpx::applier::apply<hpx::actions::dynamic_plain_action>(gid, args);
    }
}

#endif