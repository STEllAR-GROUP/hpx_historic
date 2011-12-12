//  Copyright (c) 2010 Alex Nagelberg
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_TYPES_XPI_THREAD_APR_9_2010_1055AM)
#define XPI_TYPES_XPI_THREAD_APR_9_2010_1055AM

#include <xpi.h>

struct XPI_Thread_impl
{
    XPI_Thread_impl(hpx::threads::thread_self& t, XPI_Thread_State s) 
      : thread(t), state(s) {}

    hpx::threads::thread_self& thread;
    XPI_Thread_State state;
};

#endif