//  Copyright (c) 2010 Alex Nagelberg
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_TYPES_XPI_ACTION_APR_9_2010_1055AM)
#define XPI_TYPES_XPI_ACTION_APR_9_2010_1055AM

struct XPI_Action_impl
{
    char *name;
    size_t arg_count; // number of args in array
    size_t type_size; // size of the arg_types array
    XPI_Type *arg_types;
};

#endif