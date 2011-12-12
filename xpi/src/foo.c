/*
 * Copyright (c) 2010 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <stdio.h>
#include <xpi/types/xpi_action.hpp>
#include <string.h>
#include <time.h>

// test action
XPI_Error bar(XPI_Action *action, XPI_Struct *args)
{
    fprintf(stderr, "entered in action\n");
    fprintf(stderr, "action: %s\n", action->impl->name);

    XPI_Type *arg_types=malloc(sizeof(XPI_Type)*2);
    arg_types[0]=XPI_U64_Type; // n
    arg_types[1]=XPI_U64_Type; // result

    XPI_Action newaction={0};
    char *function_name=malloc(sizeof(char)*8);
    strcpy(function_name, "foo.fib");
    XPI_Action_create(function_name, 2, 2, arg_types, &newaction);

    XPI_Struct *fib_arg=XPI_alloc(sizeof(XPI_Struct)*2);
    fib_arg[0].u64=20;

    XPI_LCO lco={0};
    XPI_Future_create(&lco);

    clock_t start=clock();
    XPI_Future_register(&lco, &newaction, fib_arg, 1);
    fprintf(stderr, "future registered. waiting on future.\n");
    XPI_Future_wait(&lco, 0);
    clock_t finish=clock();
    clock_t elapsed=finish-start;
    double elapsed_ms=(double)elapsed/(double)CLOCKS_PER_SEC;
    fprintf(stderr, "finished waiting for future.\n");
    fprintf(stderr, "result of fib(20)=%i\n", fib_arg[1].u64);
    fprintf(stderr, "elapsed=%fs\n",elapsed_ms);
    //fprintf(stderr, "data: %i\n", args[0].array.str[0][0].i16);

    //mutex test
    /*XPI_Type *arg_types=malloc(sizeof(XPI_Type)*3);
    arg_types[0]=XPI_U16_Type;
    arg_types[1]=XPI_U16_Type;
    arg_types[2]=XPI_Gid_Type;

    XPI_Action newaction={0};
    char *function_name=malloc(sizeof(char)*11);
    strcpy(function_name, "foo.thread");
    XPI_Action_create(function_name, 3, 3, arg_types, &newaction);

    XPI_LCO lco={0};
    XPI_Mutex_create(&lco);
    XPI_Struct *arg1=XPI_alloc(sizeof(XPI_Struct)*2);
    arg1[0].u16=0;
    arg1[1].u16=1;
    arg1[2].gid.id_msb_=args[0].u64;
    arg1[2].gid.id_lsb_=args[1].u64;
    /*XPI_Struct *arg2=XPI_alloc(sizeof(XPI_Struct)*2);
    arg2[0].u8=2;
    arg2[1].u64=(XPI_U64)lco.impl;
    XPI_Struct *arg3=XPI_alloc(sizeof(XPI_Struct)*2);
    arg3[0].u8=3;
    arg3[1].u64=(XPI_U64)lco.impl;*/
    /*XPI_Gid *gid=malloc(sizeof(XPI_Gid));
    gid->id_msb_=args[0].u64;
    gid->id_lsb_=args[1].u64;

    XPI_Attrs attrs = { 0 };
    XPI_Apply(attrs, &newaction, arg1, 1, gid, 0, NULL, 0, NULL);
    //XPI_Apply(attrs, &newaction, arg2, 1, gid, 0, NULL, 0, NULL);
    //XPI_Apply(attrs, &newaction, arg3, 1, gid, 0, NULL, 0, NULL);*/
    
    return XPI_SUCCESS;
}

XPI_Error thread(XPI_Action *action, XPI_Struct *args)
{
    /*fprintf(stderr, "Thread %i requesting lock...\n", args[0].u8);
    XPI_LCO lco={(struct XPI_LCO_impl *)args[1].u64};
    XPI_Mutex_register(&lco);
    XPI_Mutex_wait(&lco);
    fprintf(stderr, "Thread %i got lock. Going to sleep for a sec.\n", args[0].u8);
    sleep(2);
    fprintf(stderr, "Thread %i giving away lock.\n", args[0].u8);
    XPI_Mutex_unregister(&lco);*/
    XPI_Attrs attrs = {0};
    XPI_U16 result=args[0].u16+args[1].u16;
    if (result <= 55) // stop at 55
    {
        args[0].u16=args[1].u16;
        args[1].u16=result;
        XPI_Gid *gid=malloc(sizeof(XPI_Gid));
        gid->id_lsb_=args[2].gid.id_lsb_;
        gid->id_msb_=args[2].gid.id_msb_;
        XPI_Apply(attrs, action, args, 1, gid, 0, NULL, 0, NULL);
    }

    return XPI_SUCCESS;
}

XPI_Error fib(XPI_Action *action, XPI_Struct *args)
{

    // here is the actual fib calculation
    if (args[0].u64<2) // n < 2
        args[1].u64=args[0].u64; // result = n
    else
    {
        XPI_LCO fib_n1={0};
        XPI_LCO fib_n2={0};
        XPI_Future_create(&fib_n1);
        XPI_Future_create(&fib_n2);
        XPI_Struct *n1=XPI_alloc(sizeof(XPI_Struct)*2);
        XPI_Struct *n2=XPI_alloc(sizeof(XPI_Struct)*2);
        n1[0].u64=args[0].u64-1;
        n2[0].u64=args[0].u64-2;
        XPI_Future_register(&fib_n1, action, n1, 1);
        XPI_Future_register(&fib_n2, action, n2, 1);
        XPI_Future_wait(&fib_n1, 0);
        XPI_Future_wait(&fib_n2, 0);
        args[1].u64=n1[1].u64+n2[1].u64; // result = n1+n2
    }
    return XPI_SUCCESS;
}

XPI_Error future(XPI_Action *action, XPI_Struct *args)
{
    fprintf(stderr, "Welcome to the fuuutttuuurreeee\n");
    fprintf(stderr, "Going to sleep for a couple seconds.\n");
    sleep(5);
    fprintf(stderr, "Woken up. Returning.\n");
    return 20;
}

XPI_Error null_function(XPI_Action *action, XPI_Struct *args)
{
}
