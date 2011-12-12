/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_XPI_FUTURE_
#define _H_XPI_FUTURE_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpi.h>

XPI_API_EXPORT XPI_Error XPI_Future_create(XPI_LCO *future, XPI_Action *action, XPI_Buffer *arg, XPI_Gid *gid);
XPI_API_EXPORT XPI_Error XPI_Future_get(XPI_LCO *future, XPI_Buffer *result);
XPI_API_EXPORT XPI_Error XPI_Future_destroy(XPI_LCO *future);

#ifdef __cplusplus
}
#endif
#endif

