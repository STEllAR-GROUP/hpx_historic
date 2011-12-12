/*
 * Copyright (c) 2009 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <stdio.h>
#include "CuTest.h"

int g_argc;
char **g_argv;

XPI_Error exec_main(int argc, char *argv[])
{
    return XPI_SUCCESS;
}

XPI_Error init_main(int argc, char *argv[])
{
    return XPI_Shutdown();
}

void basic_init(CuTest *tc)
{
    int argc=g_argc;
    char **argv=g_argv;
    // Test basic manual initialization/shutdown
    CuAssert(tc, "basic initialization", XPI_Init(&argc, &argv, init_main)==XPI_SUCCESS);
}

void basic_exec(CuTest *tc)
{
    int argc=g_argc;
    char **argv=g_argv;
    // Test basic execution flow
    CuAssert(tc, "basic execution", XPI_Exec(&argc, &argv, exec_main)==XPI_SUCCESS);
}

int main(int argc, char *argv[])
{
    g_argc=argc;
    g_argv=argv;
    CuSuite *suite=CuSuiteNew();
    CuString *output=CuStringNew();
    SUITE_ADD_TEST(suite, basic_exec);
    SUITE_ADD_TEST(suite, basic_init);
    
    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    
    return suite->failCount;
}
