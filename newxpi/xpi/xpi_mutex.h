/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_XPI_MUTEX_
#define _H_XPI_MUTEX_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpi.h>
XPI_API_EXPORT XPI_Error XPI_Mutex_create(XPI_LCO *mutex, XPI_Gid *gid);
XPI_API_EXPORT XPI_Error XPI_Mutex_lock(XPI_LCO *mutex);
XPI_API_EXPORT XPI_Error XPI_Mutex_unlock(XPI_LCO *mutex);

#ifdef __cplusplus
}
#endif
#endif

