/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#include <xpi.h>
#include <xpi_lco.h>
#include "components/LCO_generic.hpp"

XPI_API_EXPORT XPI_Error XPI_LCO_Create(XPI_LCO *lco, XPI_Buffer receptor)
{
    hpx::naming::gid_type prefix(
        hpx::applier::get_applier().get_runtime_support_raw_gid());
    hpx::naming::id_type agid(
        prefix,
        hpx::naming::id_type::unmanaged);

    XPI::LCO::generic_client cli;
    cli.create(agid);
    cli.init();
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_LCO_Release(XPI_LCO *lco) // release buffer, release LCO
{
    
    return XPI_SUCCESS;
}
