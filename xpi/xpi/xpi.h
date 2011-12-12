/*
 * Copyright (c) 2009-2011 Alex Nagelberg
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

//-------------- Old API ---------------

// Types
#ifndef __cplusplus
#define bool int
#endif
    
#define XPI_ERROR -1
#define XPI_SUCCESS 0
    
#define XPI_Error int

#define XPI_F32 float
#define XPI_F64 double
#define XPI_U64 uint64_t
#define XPI_U32 uint32_t
#define XPI_U16 uint16_t
#define XPI_U8  uint8_t
#define XPI_I64 int64_t
#define XPI_I32 int32_t
#define XPI_I16 int16_t
#define XPI_I8  int8_t    


#define XPI_Type uint16_t
#define XPI_F32_Type 1
#define XPI_F64_Type 2
#define XPI_U64_Type 3
#define XPI_U32_Type 4
#define XPI_U16_Type 5
#define XPI_U8_Type 6
#define XPI_I64_Type 7
#define XPI_I32_Type 8
#define XPI_I16_Type 9
#define XPI_I8_Type 10
#define XPI_Gid_Type 11
#define XPI_Array_Type 12
#define XPI_Struct_Type 13

typedef struct
{
    uint64_t id_msb_;
    uint64_t id_lsb_;
} XPI_Gid;
    
typedef union _XPI_Array
{
    XPI_F32 *f32;
    XPI_F64 *f64;
    XPI_U64 *u64;
    XPI_U32 *u32;
    XPI_U16 *u16;
    XPI_U8 *u8;
    XPI_I64 *i64;
    XPI_I32 *i32;                
    XPI_I16 *i16;                
    XPI_I8 *i8;
    XPI_Gid *gid;
    union _XPI_Array *array;
    union _XPI_Struct **str;
} XPI_Array;

typedef union _XPI_Struct
{
    XPI_F32 f32;
    XPI_F64 f64;
    XPI_U64 u64;
    XPI_U32 u32;
    XPI_U16 u16;
    XPI_U8 u8;
    XPI_I64 i64;
    XPI_I32 i32;                
    XPI_I16 i16;                
    XPI_I8 i8;
    XPI_Gid gid;
    XPI_Array array;
    union _XPI_Struct *str;
} XPI_Struct;

typedef enum
{
    XPI_THREADSTATE_SUSPENDED,
    XPI_THREADSTATE_EXECUTING,
    XPI_THREADSTATE_DEPLETED
} XPI_Thread_State;
    
typedef struct
{
    XPI_I32 priority;
    XPI_U64 stack_size_limit;
    XPI_Gid destination;
    XPI_Thread_State initial_state;
    bool thread_movable;
    XPI_Thread_State thread_state;    
} XPI_Attrs; 
    
typedef enum
{
    XPI_Priority,
    XPI_StackSizeLimit,
    XPI_Destination,
    XPI_InitialState,
    XPI_ThreadMovable,
    XPI_ThreadState
} XPI_Attr_id;

typedef struct
{
    struct XPI_Action_impl *impl;
} XPI_Action;
    
typedef struct
{
    struct XPI_LCO_impl *impl;
} XPI_LCO;
    
typedef struct
{
    struct XPI_Thread_impl *impl;
} XPI_Thread;
    
typedef enum
{
    XPI_PARCEL_STATUS_UNKNOWN,
    XPI_PARCEL_STATUS_INTRANSIT,
    XPI_PARCEL_STATUS_DELIVERED
} XPI_Status;
    
typedef struct
{
    XPI_Action *action;
    XPI_Struct *args;
    XPI_Gid *conts;
    size_t contcnt;
    //XPI_Status stat;
} XPI_Parcel;
    
// xpi_globals.cpp
XPI_API_EXPORT XPI_Error XPI_Shutdown();
XPI_API_EXPORT XPI_Error XPI_Init(int *argc, char **argv[], XPI_Error (*xpi_main)(int, char *[]));
XPI_API_EXPORT XPI_Error XPI_Exec(int *argc, char **argv[], XPI_Error (*xpi_main)(int, char *[]));
XPI_API_EXPORT void *XPI_alloc(size_t size);
XPI_API_EXPORT XPI_Error XPI_copy(void *src, void **dest);
XPI_API_EXPORT void XPI_free(void *src);
    
// xpi_threads.cpp
XPI_API_EXPORT XPI_Error XPI_Action_create(char *name, size_t arg_count, size_t type_size, XPI_Type *types, XPI_Action *action);
XPI_API_EXPORT XPI_Error XPI_Action_delete(XPI_Action *action);
XPI_API_EXPORT XPI_Error XPI_Thread_terminate(XPI_Thread *thread);
XPI_API_EXPORT XPI_Error XPI_Thread_suspend(XPI_Thread *thread);
XPI_API_EXPORT XPI_Error XPI_Thread_self(XPI_Thread *thread);    
    
// xpi_parcels.cpp
XPI_API_EXPORT XPI_Error XPI_Apply(XPI_Attrs attrs, XPI_Action *action, XPI_Struct *args, size_t destcnt, XPI_Gid *dests, 
                                   size_t cnucnt, XPI_Gid *cnus, size_t ocncnt, XPI_Gid *ocns); //, size_t handlecnt, XPI_Parcel *handles
XPI_API_EXPORT XPI_Error XPI_Apply_unmanaged(XPI_Attrs attrs, XPI_Action *action, XPI_Struct *args, size_t destcnt, XPI_Gid *dests, 
                                   size_t cnucnt, XPI_Gid *cnus, size_t ocncnt, XPI_Gid *ocns); //, size_t handlecnt, XPI_Parcel *handles
XPI_API_EXPORT XPI_Error XPI_Parcel_get(XPI_Gid *source, size_t scnt, XPI_Status *status, size_t stcnt, XPI_Parcel *handles, size_t *hcnt);
XPI_API_EXPORT XPI_Error XPI_Parcel_free(XPI_Parcel *handle);
XPI_API_EXPORT XPI_Error XPI_Parcel_infer(XPI_Parcel handle, XPI_Action *action, XPI_Gid *args, size_t *acnt, XPI_Gid *conts, size_t contcnt);
XPI_API_EXPORT XPI_Error XPI_Parcel_status(XPI_Parcel handle, XPI_Status *stat);

// xpi_lcos.cpp
XPI_API_EXPORT XPI_Error XPI_LCO_fire(XPI_Gid *lco, double tmout, XPI_Struct *value);

// mutex_impl.hpp
XPI_API_EXPORT XPI_Error XPI_Mutex_create(XPI_LCO *lco);
XPI_API_EXPORT XPI_Error XPI_Mutex_register(XPI_LCO *lco);
XPI_API_EXPORT XPI_Error XPI_Mutex_unregister(XPI_LCO *lco);
XPI_API_EXPORT XPI_Error XPI_Mutex_wait(XPI_LCO *lco);

// future_impl.hpp
XPI_API_EXPORT XPI_Error XPI_Future_create(XPI_LCO *lco);
XPI_API_EXPORT XPI_Error XPI_Future_register(XPI_LCO *lco, XPI_Action *action, XPI_Struct *arg, bool eager); // lazy futures can delay computation till wait, eager will start as soon as registered
XPI_API_EXPORT XPI_Error XPI_Future_unregister(XPI_LCO *lco);
XPI_API_EXPORT XPI_Error XPI_Future_wait(XPI_LCO *lco, void *result); // put thread to sleep, wait for result

//-------------------------------------



// ----------- New API ----------------
typedef struct
{
    uint8_t *buf;
    struct XPI_Buffer_impl *impl;
} XPI_Buffer;

// xpi_buffer.cpp
XPI_API_EXPORT XPI_Error XPI_Buffer_create(XPI_Buffer **buf, size_t size, uint8_t flags);
XPI_API_EXPORT XPI_Error XPI_Buffer_copy(XPI_Buffer *src, XPI_Buffer **dest);
XPI_API_EXPORT XPI_Error XPI_Buffer_release(XPI_Buffer *buf);

#ifdef __cplusplus
}
#endif
#endif
