/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <xpi_action.h>
#include <xpi_future.h>
#include <stdio.h>

XPI_Error xpi_main(int argc, char *argv[])
{
    fprintf(stderr, "xpi_main init.\n");

    XPI_Gid *locality=malloc(sizeof(XPI_Gid));
    XPI_Runtime_gid(locality);

    XPI_Buffer *buffer=malloc(sizeof(XPI_Buffer)); 
    XPI_Buffer_create(buffer, 1024, 0);

    XPI_Action *action=malloc(sizeof(XPI_Action));
    XPI_Action_create("foo.bar", action);

    XPI_LCO future;
    fprintf(stderr, "creating future\n");
    XPI_Future_create(&future, action, buffer, locality);
    fprintf(stderr, "calling future.get()\n");
    XPI_Future_get(&future, buffer);
    fprintf(stderr, "future returned\n");
    return XPI_SUCCESS;
}

int main(int argc, char *argv[])
{
    XPI_Exec(&argc, &argv, xpi_main);
    return 0;
}

