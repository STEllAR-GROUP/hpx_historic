/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#include <stdio.h>
#include <xpi.h>
#include <xpi_action.h>
#include <xpi_future.h>
#include <string.h>
#include <time.h>

typedef struct
{
    int n;
    long result;
} fib_args;

XPI_Error bar(XPI_Action *action, XPI_Buffer *arg)
{
    XPI_Gid locality;
    XPI_Runtime_gid(&locality);

    XPI_Buffer *n1=malloc(sizeof(XPI_Buffer));
    XPI_Buffer_create(n1,sizeof(fib_args),0);
    
    ((fib_args *)n1->buf)->result=0;
    ((fib_args *)n1->buf)->n=20;
    
    XPI_Action *newaction=malloc(sizeof(XPI_Action));
    char *function_name=malloc(sizeof(char)*8);
    strcpy(function_name, "foo.fib");
    XPI_Action_create(function_name, newaction);

    clock_t start=clock();
    XPI_LCO *future=malloc(sizeof(XPI_LCO));
    XPI_Future_create(future, newaction, n1, &locality);
    XPI_Future_get(future, n1);
    clock_t finish=clock();
    clock_t elapsed=finish-start;
    double elapsed_ms=(double)elapsed/(double)CLOCKS_PER_SEC; 
    fprintf(stderr, "result of fib(20)=%ld\n", ((fib_args *)n1->buf)->result);
    fprintf(stderr, "elapsed: %fs\n", elapsed_ms);

    XPI_Future_destroy(future);
    free(future);
    XPI_Buffer_release(n1);
    free(n1);
    XPI_Action_destroy(newaction);
    free(newaction);

    return XPI_SUCCESS;
}

XPI_Error fib(XPI_Action *action, XPI_Buffer *arg)
{
    return XPI_SUCCESS;
    XPI_Gid gid;
    XPI_Runtime_gid(&gid);

    fib_args *input=(fib_args *)arg->buf;

    if (input->n < 2)
        input->result=input->n;
    else
    {
        XPI_Buffer *n1=malloc(sizeof(XPI_Buffer));
        XPI_Buffer *n2=malloc(sizeof(XPI_Buffer));
        
        XPI_Buffer_create(n1,sizeof(fib_args),0);
        XPI_Buffer_create(n2,sizeof(fib_args),0);
        
        ((fib_args *)n1->buf)->result=0;
        ((fib_args *)n1->buf)->n=input->n-1;
        ((fib_args *)n2->buf)->result=0;
        ((fib_args *)n2->buf)->n=input->n-2;

        XPI_LCO *future1=malloc(sizeof(XPI_LCO));
        XPI_Future_create(future1, action, n1, &gid);
        XPI_Future_get(future1, n1);
  
        XPI_LCO *future2=malloc(sizeof(XPI_LCO));
        XPI_Future_create(future2, action, n2, &gid);

        //XPI_Future_get(future1, n1); 
        XPI_Future_get(future2, n2);
        input->result=((fib_args *)n1->buf)->result + ((fib_args *)n2->buf)->result;        

        XPI_Future_destroy(future1);
        XPI_Future_destroy(future2);
        free(future1);
        free(future2);

        XPI_Buffer_release(n1);
        XPI_Buffer_release(n2);
        free(n1);
        free(n2);
    }
    return XPI_SUCCESS;
}
