/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _H_XPI_Buffer_impl_
#define _H_XPI_Buffer_impl_

#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>

class XPI_Buffer_impl
{
private:
    int ref_count;
    size_t size;
    uint8_t *buf;
    uint8_t flags;

public:
    uint8_t *get_buffer()
    {
        return buf;
    }

    int increment_ref_count()
    {
        return ++ref_count;
    }

    int decrement_ref_count()
    {
        ref_count--;
        if (ref_count<=0)
            delete[] buf;

        return ref_count;
    }

    size_t get_size()
    {
        return size;
    }

    XPI_Buffer_impl(size_t _size, uint8_t _flags)
    {
        size=_size;
        buf=new uint8_t[size];
        flags=_flags;
        ref_count=1;
    }

    // replace all these with smart ptr action
    XPI_Buffer_impl()
    {}

    ~XPI_Buffer_impl()
    {
    }

private: 
    friend class boost::serialization::access;

    template<class Archive>
    void save(Archive &ar, const unsigned int version) const
    {   
        ar & flags; 
        ar & size;
        // TODO: may not need to do this
        ar.save_binary(buf, size);
    }

    template<class Archive>
    void load(Archive &ar, const unsigned int version)
    {   
        ar & flags;
        ar & size;
        buf = new uint8_t[size];
        ar.load_binary(buf, size);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    
};

#endif
