/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _XPI_H_
#define _XPI_H_

#include <stdlib.h>
#include <stdint.h>
#include <xpi_exports.h>
#ifdef __cplusplus
extern "C"
{
#endif

// Types
#ifndef __cplusplus
#define bool int
#endif
    
#define XPI_ERROR -1
#define XPI_SUCCESS 0
    
#define XPI_Error int

typedef struct
{
//    uint64_t id_msb_;
//    uint64_t id_lsb_;
    struct XPI_Gid_impl *impl;
} XPI_Gid;
    
    
typedef struct
{
    uint8_t *buf;
    struct XPI_Buffer_impl *impl;
} XPI_Buffer;

typedef struct
{
    struct XPI_Action_impl *impl;
} XPI_Action;

typedef struct
{
    struct XPI_Future_impl *impl;
} XPI_Future;

typedef struct
{
    struct XPI_LCO_impl *impl;
} XPI_LCO;

/* deprecated
// xpi_eager_future.cpp
XPI_API_EXPORT XPI_Error XPI_Eager_Future_create(XPI_Future *future, XPI_Action *action, XPI_Buffer *arg, XPI_Gid *gid);
XPI_API_EXPORT XPI_Error XPI_Eager_Future_wait(XPI_Future *future);
XPI_API_EXPORT XPI_Error XPI_Eager_Future_destroy(XPI_Future *future);
*/
  
// xpi_globals.cpp
XPI_API_EXPORT XPI_Error XPI_Shutdown();
XPI_API_EXPORT XPI_Error XPI_Init(int *argc, char **argv[], XPI_Error (*xpi_main)(int, char *[]));
XPI_API_EXPORT XPI_Error XPI_Exec(int *argc, char **argv[], XPI_Error (*xpi_main)(int, char *[]));
XPI_API_EXPORT XPI_Error XPI_Runtime_gid(XPI_Gid *gid);
    
// xpi_buffer.cpp
XPI_API_EXPORT XPI_Error XPI_Buffer_create(XPI_Buffer *buf, size_t size, uint8_t flags);
XPI_API_EXPORT XPI_Error XPI_Buffer_access(XPI_Buffer *src);
XPI_API_EXPORT XPI_Error XPI_Buffer_release(XPI_Buffer *buf);

#ifdef __cplusplus
}
#endif
#endif
