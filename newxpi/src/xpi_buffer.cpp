/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <xpi_buffer.hpp>


// TODO: replace with smart ptr

XPI_API_EXPORT XPI_Error XPI_Buffer_create(XPI_Buffer *buf, size_t size, uint8_t flags)
{
    buf->impl=new XPI_Buffer_impl(size, flags);
    if (!buf->impl)
        return XPI_ERROR;

    buf->buf=buf->impl->get_buffer();
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Buffer_access(XPI_Buffer *buf)
{
    buf->impl->increment_ref_count();
    //dest->impl=src->impl; // TODO: this could do copy constructor magic
    //dest->buf=src->buf;
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Buffer_release(XPI_Buffer *buf)
{
    if (buf->impl->decrement_ref_count()==0)
        delete buf->impl;
    
    return XPI_SUCCESS;
}
