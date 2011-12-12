/*
 * Copyright (c) 2009 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <xpi/types/gid_list.hpp>
#include <xpi/components/multicaster.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/actions/dynamic_plain_action.hpp>
#include <hpx/runtime/actions/continuation.hpp>
#include <string>

void _XPI_Apply(size_t destcnt, XPI_Gid *dests, hpx::actions::dynamic_argument &arg, xpi::types::gid_list &continuations)
{
    size_t i;
    for (i=0; i<destcnt; i++)            
    {
        hpx::naming::id_type gid(dests[i].id_msb_, dests[i].id_lsb_, hpx::naming::id_type::unmanaged);        
        if (continuations.size()>0)
        {
            xpi::components::multicaster::client cli;
            cli.create(gid);
            cli.init(continuations, arg);
            hpx::applier::apply_c<hpx::actions::dynamic_plain_action>(cli.get_gid(), gid, arg);
        }
        else
            hpx::applier::apply<hpx::actions::dynamic_plain_action>(gid, arg);
    }
}

XPI_Error XPI_Apply(XPI_Attrs attrs, XPI_Action *action, XPI_Struct *args, size_t destcnt, XPI_Gid *dests, 
                    size_t cnucnt, XPI_Gid *cnus, size_t ocncnt, XPI_Gid *ocns)
{
    hpx::actions::dynamic_argument arg(action->impl, args, ocncnt, ocns, true);
    xpi::types::gid_list continuations(cnus, cnucnt);
    
    _XPI_Apply(destcnt, dests, arg, continuations);
    return XPI_SUCCESS;
}

XPI_Error XPI_Apply_unmanaged(XPI_Attrs attrs, XPI_Action *action, XPI_Struct *args, size_t destcnt, XPI_Gid *dests, 
                              size_t cnucnt, XPI_Gid *cnus, size_t ocncnt, XPI_Gid *ocns)
{
    hpx::actions::dynamic_argument arg(action->impl, args, ocncnt, ocns, false);
    xpi::types::gid_list continuations(cnus, cnucnt);
    
    _XPI_Apply(destcnt, dests, arg, continuations);
    return XPI_SUCCESS;
}

XPI_Error XPI_Parcel_get(XPI_Gid *source, size_t scnt, XPI_Status *status, size_t stcnt, XPI_Parcel *handles, size_t *hcnt)
{    
    return XPI_SUCCESS;
}

XPI_Error XPI_Parcel_free(XPI_Parcel *handle)
{
    return XPI_SUCCESS;
}

XPI_Error XPI_Parcel_infer(XPI_Parcel handle, XPI_Action **action, XPI_Struct **args, XPI_Gid **conts, size_t *contcnt)
{
    *action=handle.action;
    *args=handle.args;
    *conts=handle.conts;
    *contcnt=handle.contcnt;
    return XPI_SUCCESS;
}

XPI_Error XPI_Parcel_status(XPI_Parcel handle, XPI_Status *stat)
{
    //*stat=handle.stat;
    return XPI_SUCCESS;
}
