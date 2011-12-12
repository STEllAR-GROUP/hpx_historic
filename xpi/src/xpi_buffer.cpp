/*
 * Copyright (c) 2009-2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>

struct XPI_Buffer_impl
{
    int ref_count;
    size_t size;
    uint8_t flags;
};

XPI_API_EXPORT XPI_Error XPI_Buffer_create(XPI_Buffer **buf, size_t size, uint8_t flags)
{
    XPI_Buffer *tmp=new XPI_Buffer;
    if (!tmp) return XPI_ERROR;

    tmp->impl=new XPI_Buffer_impl;
    if (!tmp->impl)
    {
        delete tmp;
        return XPI_ERROR;
    }

    tmp->impl->ref_count=1;
    tmp->impl->size=size;
    tmp->impl->flags=flags;

    tmp->buf=new uint8_t[size];
    if (!tmp->buf)
    {
        delete tmp->impl;
        delete tmp;
        return XPI_ERROR;
    }

    buf=tmp;
    return XPI_SUCCESS;

}

XPI_API_EXPORT XPI_Error XPI_Buffer_copy(XPI_Buffer *src, XPI_Buffer **dest)
{
    src->impl->ref_count++;
    *dest=src;
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Buffer_release(XPI_Buffer *buf)
{
    buf->impl->ref_count--;
    if (buf->impl->ref_count==0)
    {
        delete buf->impl;
        delete[] buf->buf;
        delete buf;
    }

    return XPI_SUCCESS;
}
