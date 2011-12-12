/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_XPI_ACTION_
#define _H_XPI_ACTION_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpi.h>

XPI_API_EXPORT XPI_Error XPI_Action_create(const char *name, XPI_Action *action);
XPI_API_EXPORT XPI_Error XPI_Action_destroy(XPI_Action *action);

#ifdef __cplusplus
}
#endif
#endif
