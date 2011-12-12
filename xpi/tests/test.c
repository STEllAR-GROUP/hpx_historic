/*
 * Copyright (c) 2009 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <stdio.h>

XPI_Error xpi_main(int argc, char *argv[])
{
    int i;
    for (i=0; i<argc; i++)
    {
        fprintf(stderr, "%s\n", argv[i]);
    }
    return XPI_SUCCESS;
}

int main(int argc, char *argv[])
{
    XPI_Exec(&argc, &argv, xpi_main);
    return 0;
}
