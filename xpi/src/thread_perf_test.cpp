/*
 * Copyright (c) 2009 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <stdio.h>
#include <hpx/hpx.hpp>

XPI_Error xpi_main(int argc, char *argv[])
{
    hpx::applier::applier& appl = hpx::applier::get_applier();

    std::vector<hpx::naming::gid_type> prefixes;
    hpx::naming::gid_type this_prefix = appl.get_runtime_support_raw_gid();
    hpx::naming::gid_type that_prefix;
    if (appl.get_remote_prefixes(prefixes)) {
        that_prefix = prefixes[0];
    }
    else {
        that_prefix = this_prefix;
    }
    std::cout << "this prefix: " << this_prefix << "\nthat prefix: " << that_prefix << "\n";

    XPI_Gid *gid=new XPI_Gid[1];
    gid[0].id_msb_=that_prefix.get_msb();
    gid[0].id_lsb_=that_prefix.get_lsb();

    XPI_Type *arg_types=new XPI_Type[1];
    arg_types[0]=XPI_U8_Type;

    XPI_Action action={0};
    char *function_name=new char[15];
    strcpy(function_name, "foo.null_function");
    XPI_Action_create(function_name, 1, 1, arg_types, &action);
    XPI_Attrs attrs = { 0 };

    XPI_Struct *arg=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct));

    // get number of threads to do from argv, do this in loop
    long num_threads=atol(argv[1]);
    for (long i=0; i<num_threads; ++i)
        XPI_Apply_unmanaged(attrs, &action, arg, 1, gid, 0, NULL, 0, NULL);
}

int main(int argc, char *argv[])
{
    XPI_Exec(&argc, &argv, xpi_main);
    return 0;
}
