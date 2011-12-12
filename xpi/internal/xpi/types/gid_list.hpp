//  Copyright (c) 2010 Alex Nagelberg
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_TYPES_GID_LIST_MAR_5_2010_1113AM)
#define XPI_TYPES_GID_LIST_MAR_5_2010_1113AM

#include <xpi.h>
#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>

#include <hpx/config/warnings_prefix.hpp>

namespace xpi { namespace types
{
    class gid_list_impl
    {
    private:
        friend class gid_list;
        XPI_Gid *gids;
        size_t gid_cnt;
    public:
        gid_list_impl() {}
        gid_list_impl(XPI_Gid *_gids, size_t _gid_cnt) : gids(_gids), gid_cnt(_gid_cnt) {}
        
        ~gid_list_impl()
        {
            //delete[] gids;
        }
    private:
        friend class boost::serialization::access;
        
        template<class Archive>
        void save(Archive &ar, const unsigned int version) const
        {
            ar & gid_cnt;
            size_t i;
            for (i=0; i<gid_cnt; ++i)
            {
                ar & gids[i].id_msb_;
                ar & gids[i].id_lsb_;
            }
        }
        
        template<class Archive>
        void load(Archive &ar, const unsigned int version)
        {
            ar & gid_cnt;
            gids=new XPI_Gid[gid_cnt];
            
            size_t i;
            for (i=0; i<gid_cnt; ++i)
            {
                ar & gids[i].id_msb_;
                ar & gids[i].id_lsb_;
            }
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
    
    class gid_list
    {
    private:
        boost::shared_ptr<gid_list_impl> ptr;
    
    public:
        gid_list() : ptr(new gid_list_impl()) {}
        gid_list(const gid_list &gl) : ptr(gl.ptr) {}
        gid_list(XPI_Gid *_gids, size_t _gid_cnt) : ptr(new gid_list_impl(_gids, _gid_cnt)) {}
        
        XPI_Gid *get_gids()
        {
            return ptr->gids;
        }
        
        size_t size()
        {
            return ptr->gid_cnt;
        }
        
    private:
        friend class boost::serialization::access;
        
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version) {
            ar & *ptr;
        }
    };
}}

#include <hpx/config/warnings_suffix.hpp>

#endif
