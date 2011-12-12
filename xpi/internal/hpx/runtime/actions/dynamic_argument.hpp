//  Copyright (c) 2007-2009 Hartmut Kaiser, Alex Nagelberg
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_HPX_RUNTIME_ACTIONS_DYNAMIC_ARGUMENT_OCT_28_2009_1101AM)
#define XPI_HPX_RUNTIME_ACTIONS_DYNAMIC_ARGUMENT_OCT_28_2009_1101AM

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <hpx/runtime/actions/action_registry.hpp>
#include <xpi.h>
#include <iostream>
#include <boost/serialization/string.hpp>
#include <xpi/types/xpi_action.hpp>

#include <hpx/config/warnings_prefix.hpp>

///////////////////////////////////////////////////////////////////////////////
// Version of id_type
#define XPI_DYNAMIC_ARGUMENT_VERSION 0x10

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace actions
{
    ///////////////////////////////////////////////////////////////////////////
    class dynamic_argument_impl
    {
    private: 
        friend class dynamic_argument;
        std::list< boost::shared_ptr<void> > arrays_to_free;
    public: XPI_Struct *args;
    private:    XPI_Action_impl *action;
        bool sender;
        size_t ocncnt;
        XPI_Gid *ocns;
        bool managed;
        
        template <typename T>
        struct array_deleter
        {            
            void operator() (T *p)
            {
                delete[] p;
            }
        };
        
        template <typename T>
        struct xpi_deleter
        {            
            void operator() (T *p)
            {
                XPI_free(p);
            }
        };
        
        void delete_struct(XPI_Struct *str, size_t &pos)
        {
            size_t i;
            size_t arg_count=action->arg_types[++pos];
            for (i=0; i<arg_count; ++i)
            {
                if (action->arg_types[pos]==XPI_Struct_Type)
                    delete_struct(str[i].str, ++pos);
                else if (action->arg_types[pos]==XPI_Array_Type)
                    delete_array(str[i].array, ++pos);
                else
                    ++pos;
            }
            XPI_free(str);
        }
        
        void delete_array(XPI_Array &array, size_t &pos)
        {
            size_t array_size=action->arg_types[++pos];
            if (action->arg_types[++pos]==XPI_Struct_Type)
            {
                size_t i;
                for (i=0; i<array_size; ++i)
                    delete_struct(array.str[i], pos);
            }
            XPI_free(array.u64);
        }
        
    public:           
        dynamic_argument_impl()
        {
            sender=false;
            managed=false;
        }
        
        ~dynamic_argument_impl()
        {
            if (sender && managed)
            {
                size_t i, j=0;
                for (i=0; i<action->arg_count; ++i)
                {
                    if (action->arg_types[j]==XPI_Struct_Type)
                        delete_struct(args[i].str, j);
                    else if (action->arg_types[j]==XPI_Array_Type)
                        delete_array(args[i].array, j);                    
                    ++j;
                }                
                XPI_free(args);
            }
        }        
        
        dynamic_argument_impl(XPI_Action_impl *_action, XPI_Struct *_args, size_t _ocncnt, XPI_Gid *_ocns, bool _managed)
        {
            sender=true;
            managed=_managed;
            args=_args;
            if (managed) arrays_to_free.push_back(boost::shared_ptr<void>(args, xpi_deleter<XPI_Struct>()));
            action=_action;
            ocncnt=_ocncnt;
            ocns=_ocns;
            arrays_to_free.push_back(boost::shared_ptr<void>(ocns, array_deleter<XPI_Gid>()));
        }
        
        XPI_Error invoke_action()
        {
            hpx::naming::id_type empty_gid;
            if (!sender)
            {
                // XPI_LCO_fire
                
            }            
            XPI_Action _action={action};  
            return hpx::actions::action_registry::get_registry().get_function(action->name).get()(&_action, args);
        }
        
        bool isSender()
        {
            return sender;
        }
        
    private:
        
        friend class boost::serialization::access;
        
        template<class Archive>
        void serialize_array(Archive &ar, XPI_Array &array, size_t &pos) const
        {                                   
            size_t array_size=action->arg_types[++pos];
            XPI_Type array_type=action->arg_types[++pos]; 
            ar & array_size;
            ar & array_type;            
            switch (array_type)
            {
                case XPI_F32_Type:
                    ar.save_binary(array.f32, array_size*sizeof(XPI_F32));                        
                    break;
                case XPI_F64_Type:                       
                    ar.save_binary(array.f64, array_size*sizeof(XPI_F64));
                    break;
                case XPI_U64_Type:
                    ar.save_binary(array.u64, array_size*sizeof(XPI_U64));
                    break;
                case XPI_U32_Type:
                    ar.save_binary(array.u32, array_size*sizeof(XPI_U32));
                    break;
                case XPI_U16_Type:
                    ar.save_binary(array.u16, array_size*sizeof(XPI_U16));
                    break;
                case XPI_U8_Type:
                    ar.save_binary(array.u8, array_size*sizeof(XPI_U8));
                    break;
                case XPI_I64_Type:
                    ar.save_binary(array.i64, array_size*sizeof(XPI_I64));
                    break;
                case XPI_I32_Type:
                    ar.save_binary(array.i32, array_size*sizeof(XPI_I32));
                    break;
                case XPI_I16_Type:
                    ar.save_binary(array.i16, array_size*sizeof(XPI_I16));
                    break;
                case XPI_I8_Type:
                    ar.save_binary(array.i8, array_size*sizeof(XPI_I8));
                    break;  
                case XPI_Gid_Type:
                {
                    size_t i;
                    for (i=0; i<array_size; ++i)
                    {
                        ar & array.gid[i].id_msb_;
                        ar & array.gid[i].id_lsb_;
                    }
                    break;
                }
                case XPI_Array_Type:
                {
                    size_t i;
                    for (i=0; i<array_size; ++i)
                    {
                        serialize_array(ar, array.array[i], pos);
                    }
                    break;
                }
                case XPI_Struct_Type:         
                {
                    size_t i;
                    for (i=0; i<array_size; ++i)
                        serialize_struct(ar, array.str[i], pos);
                    break;
                }
            }
        }
        
        template<class Archive>
        void serialize_struct(Archive &ar, XPI_Struct *str, size_t &pos) const
        {            
            ar & action->arg_types[++pos];
            size_t i;
            size_t arg_count=action->arg_types[pos];
            for (i=0; i<arg_count; ++i)
            {
                ar & action->arg_types[++pos];
                switch (action->arg_types[pos])
                {
                    case XPI_F32_Type:
                        ar & str[i].f32;
                        break;
                    case XPI_F64_Type:
                        ar & str[i].f64;
                        break;
                    case XPI_U64_Type:
                        ar & str[i].u64;
                        break;
                    case XPI_U32_Type:
                        ar & str[i].u32;
                        break;
                    case XPI_U16_Type:
                        ar & str[i].u16;
                        break;
                    case XPI_U8_Type:
                        ar & str[i].u8;
                        break;
                    case XPI_I64_Type:
                        ar & str[i].i64;
                        break;
                    case XPI_I32_Type:
                        ar & str[i].i32;
                        break;
                    case XPI_I16_Type:
                        ar & str[i].i16;
                        break;
                    case XPI_I8_Type:
                        ar & str[i].i8;
                        break;
                    case XPI_Gid_Type:
                        ar & str[i].gid.id_msb_;
                        ar & str[i].gid.id_lsb_;
                        break;
                    case XPI_Array_Type:
                        serialize_array(ar, str[i].array, pos);
                        break;
                    case XPI_Struct_Type:                        
                        serialize_struct(ar, str[i].str, pos);
                        break;
                }
            }
        }
        
        template<class Archive>
        void save(Archive &ar, const unsigned int version) const
        {
            ar & managed;
            ar & ocncnt;
            if (ocncnt>0)
            {
                size_t i=0;
                for (i=0; i<ocncnt; ++i)
                {
                    ar & ocns[i].id_msb_;
                    ar & ocns[i].id_lsb_;
                }
            }
                
            std::string name(action->name);
            ar & name;
            ar & action->arg_count;
            ar & action->type_size;
            size_t i=0, j=0;
            for (i=0; i<action->arg_count; ++i)   
            {
                ar & action->arg_types[j];                
                switch (action->arg_types[j])
                {
                    case XPI_F32_Type:
                        ar & args[i].f32;                        
                        break;
                    case XPI_F64_Type:
                        ar & args[i].f64;
                        break;
                    case XPI_U64_Type:
                        ar & args[i].u64;
                        break;
                    case XPI_U32_Type:
                        ar & args[i].u32;
                        break;
                    case XPI_U16_Type:
                        ar & args[i].u16;
                        break;
                    case XPI_U8_Type:
                        ar & args[i].u8;
                        break;
                    case XPI_I64_Type:
                        ar & args[i].i64;
                        break;
                    case XPI_I32_Type:
                        ar & args[i].i32;
                        break;
                    case XPI_I16_Type:
                        ar & args[i].i16;
                        break;
                    case XPI_I8_Type:
                        ar & args[i].i8;
                        break;
                    case XPI_Gid_Type:
                        ar & args[i].gid.id_msb_;
                        ar & args[i].gid.id_lsb_;
                        break;
                    case XPI_Array_Type:                        
                        serialize_array(ar, args[i].array, j);
                        break;
                    case XPI_Struct_Type:                        
                        serialize_struct(ar, args[i].str, j);
                        break;
                }
                ++j;
            }
        }
        
        // TODO: check for infinite referencing condition in arrays
        template<class Archive>
        void deserialize_array(Archive &ar, XPI_Array &array, size_t &pos)
        {              
            ar & action->arg_types[++pos];
            size_t array_size=action->arg_types[pos];
            ar & action->arg_types[++pos];
            XPI_Type array_type=action->arg_types[pos];            
            
            switch (array_type)
            {
                case XPI_F32_Type:                    
                    array.f32=(XPI_F32 *)XPI_alloc(sizeof(XPI_F32)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.f32, xpi_deleter<XPI_F32>()));
                    ar.load_binary(array.f32, array_size*sizeof(XPI_F32));                        
                    break;
                case XPI_F64_Type:       
                    array.f64=(XPI_F64 *)XPI_alloc(sizeof(XPI_F64)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.f64, xpi_deleter<XPI_F64>()));
                    ar.load_binary(array.f64, array_size*sizeof(XPI_F64));
                    break;
                case XPI_U64_Type:              
                    array.u64=(XPI_U64 *)XPI_alloc(sizeof(XPI_U64)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.u64, xpi_deleter<XPI_U64>()));
                    ar.load_binary(array.u64, array_size*sizeof(XPI_U64));
                    break;
                case XPI_U32_Type:                    
                    array.u32=(XPI_U32 *)XPI_alloc(sizeof(XPI_U32)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.u32, xpi_deleter<XPI_U32>()));
                    ar.load_binary(array.u32, array_size*sizeof(XPI_U32));
                    break;
                case XPI_U16_Type:
                    array.u16=(XPI_U16 *)XPI_alloc(sizeof(XPI_U16)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.u16, xpi_deleter<XPI_U16>()));
                    ar.load_binary(array.u16, array_size*sizeof(XPI_U16));
                    break;
                case XPI_U8_Type:
                    array.u8=(XPI_U8 *)XPI_alloc(sizeof(XPI_U8)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.u8, xpi_deleter<XPI_U8>()));
                    ar.load_binary(array.u8, array_size*sizeof(XPI_U8));
                    break;
                case XPI_I64_Type:
                    array.i64=(XPI_I64 *)XPI_alloc(sizeof(XPI_I64)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.i64, xpi_deleter<XPI_I64>()));
                    ar.load_binary(array.i64, array_size*sizeof(XPI_I64));
                    break;
                case XPI_I32_Type:
                    array.i32=(XPI_I32 *)XPI_alloc(sizeof(XPI_I32)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.i32, xpi_deleter<XPI_I32>()));
                    ar.load_binary(array.i32, array_size*sizeof(XPI_I32));
                    break;
                case XPI_I16_Type:
                    array.i16=(XPI_I16 *)XPI_alloc(sizeof(XPI_I16)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.i16, xpi_deleter<XPI_I16>()));
                    ar.load_binary(array.i16, array_size*sizeof(XPI_I16));
                    break;
                case XPI_I8_Type:
                    array.i8=(XPI_I8 *)XPI_alloc(sizeof(XPI_I8)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.i8, xpi_deleter<XPI_I8>()));
                    ar.load_binary(array.i8, array_size*sizeof(XPI_I8));
                    break;
                case XPI_Array_Type:
                {
                    array.array=(XPI_Array *)XPI_alloc(sizeof(XPI_Array)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.array, xpi_deleter<XPI_Array>()));
                    size_t i;
                    for (i=0; i<array_size; ++i)
                    {
                        deserialize_array(ar, array.array[i], pos);
                    }
                    break;
                }
                case XPI_Struct_Type:
                {
                    typedef XPI_Struct* XPI_Struct_array;
                    array.str=(XPI_Struct_array *)XPI_alloc(sizeof(XPI_Struct_array)*array_size);
                    arrays_to_free.push_back(boost::shared_ptr<void>(array.str, xpi_deleter<XPI_Struct_array>()));
                    size_t i;
                    for (i=0; i<array_size; ++i)
                    {
                        ar & action->arg_types[++pos];
                        array.str[i]=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct)*action->arg_types[pos]);
                        arrays_to_free.push_back(boost::shared_ptr<void>(array.str[i], xpi_deleter<XPI_Struct>()));
                        deserialize_struct(ar, array.str[i], pos);
                    }
                    break;
                }
            }
        }
        
        template<class Archive>
        void deserialize_struct(Archive &ar, XPI_Struct *str, size_t &pos)
        {
            size_t i;
            size_t arg_count=action->arg_types[pos];
            for (i=0; i<arg_count; ++i)
            {                
                ar & action->arg_types[++pos];    
                switch (action->arg_types[pos])
                {
                    case XPI_F32_Type:
                        ar & str[i].f32;
                        break;
                    case XPI_F64_Type:
                        ar & str[i].f64;
                        break;
                    case XPI_U64_Type:
                        ar & str[i].u64;
                        break;
                    case XPI_U32_Type:
                        ar & str[i].u32;
                        break;
                    case XPI_U16_Type:
                        ar & str[i].u16;
                        break;
                    case XPI_U8_Type:
                        ar & str[i].u8;
                        break;
                    case XPI_I64_Type:
                        ar & str[i].i64;
                        break;
                    case XPI_I32_Type:
                        ar & str[i].i32;
                        break;
                    case XPI_I16_Type:                        
                        ar & str[i].i16;
                        break;
                    case XPI_I8_Type:
                        ar & str[i].i8;
                        break;
                    case XPI_Gid_Type:
                        ar & str[i].gid.id_msb_;
                        ar & str[i].gid.id_lsb_;
                        break;
                    case XPI_Array_Type:
                        deserialize_array(ar, str[i].array, pos);
                        break;
                    case XPI_Struct_Type:        
                        ar & action->arg_types[++pos];
                        str[i].str=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct) * action->arg_types[pos]);
                        arrays_to_free.push_back(boost::shared_ptr<void>(str[i].str, xpi_deleter<XPI_Struct>()));                        
                        deserialize_struct(ar, str[i].str, pos);
                        break;
                }
            }
        }        
        
        template<class Archive>
        void load(Archive &ar, const unsigned int version)
        {      
            ar & managed;
            ar & ocncnt;
            if (ocncnt>0)
            {
                ocns=new XPI_Gid[ocncnt];
                arrays_to_free.push_back(boost::shared_ptr<void>(ocns, array_deleter<XPI_Gid>()));
                size_t i=0;
                for (i=0; i<ocncnt; ++i)
                {
                    ar & ocns[i].id_msb_;
                    ar & ocns[i].id_lsb_;
                }
            }
            
            action=new XPI_Action_impl[1];
            arrays_to_free.push_back(boost::shared_ptr<void>(action, array_deleter<XPI_Action_impl>()));

            std::string name;
            ar & name;
            
            action->name=new char[name.length()+1];
            arrays_to_free.push_back(boost::shared_ptr<void>(action->name, array_deleter<char>()));
            strcpy(action->name, name.c_str()); // TODO: change to fixed length copy            
            
            ar & action->arg_count;         
            ar & action->type_size;
            
            args=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct) * action->type_size);
            arrays_to_free.push_back(boost::shared_ptr<void>(args, xpi_deleter<XPI_Struct>()));
            action->arg_types=new XPI_Type[action->arg_count];
            arrays_to_free.push_back(boost::shared_ptr<void>(action->arg_types, array_deleter<XPI_Type>()));
            
            size_t i=0, j=0;
            for (i=0; i<action->arg_count; ++i)            
            {                
                ar & action->arg_types[j];
                switch (action->arg_types[j])
                {
                    case XPI_F32_Type:
                        ar & args[i].f32;                        
                        break;
                    case XPI_F64_Type:
                        ar & args[i].f64;
                        break;
                    case XPI_U64_Type:
                        ar & args[i].u64;
                        break;
                    case XPI_U32_Type:
                        ar & args[i].u32;
                        break;
                    case XPI_U16_Type:
                        ar & args[i].u16;
                        break;
                    case XPI_U8_Type:
                        ar & args[i].u8;
                        break;
                    case XPI_I64_Type:
                        ar & args[i].i64;
                        break;
                    case XPI_I32_Type:
                        ar & args[i].i32;
                        break;
                    case XPI_I16_Type:
                        ar & args[i].i16;
                        break;
                    case XPI_I8_Type:
                        ar & args[i].i8;
                        break;
                    case XPI_Gid_Type:
                        ar & args[i].gid.id_msb_;
                        ar & args[i].gid.id_lsb_;
                        break;
                    case XPI_Array_Type:                                                              
                        deserialize_array(ar, args[i].array, j);                        
                        break;
                    case XPI_Struct_Type:                        
                        ar & action->arg_types[++j];
                        args[i].str=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct) * action->arg_types[j]);
                        arrays_to_free.push_back(boost::shared_ptr<void>(args[i].str, xpi_deleter<XPI_Struct>()));
                        deserialize_struct(ar, args[i].str, j);
                        break;                        
                }
                ++j;
            }    
        }
        
        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
    
    class dynamic_argument
    {
        //private:
    public:
        boost::shared_ptr<dynamic_argument_impl> ptr;        
        
    public:    
        dynamic_argument() : ptr(new dynamic_argument_impl()) {}        
        dynamic_argument(const dynamic_argument &da) : ptr(da.ptr) {  }        
        dynamic_argument(XPI_Action_impl *_action, XPI_Struct *_args, size_t ocncnt, XPI_Gid *ocns, bool managed) : ptr(new dynamic_argument_impl(_action, _args, ocncnt, ocns, managed)) {}
    
        XPI_Error invoke_action() const
        {
            return ptr->invoke_action();
        }
    private:
        friend class boost::serialization::access;
        
        template<class Archive>
        void save(Archive &ar, const unsigned int version) const
        {
            ar & *ptr;
        }
        
        template<class Archive>
        void load(Archive &ar, const unsigned int version)
        {
            ar & *ptr;
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
}}

#include <hpx/config/warnings_suffix.hpp>

///////////////////////////////////////////////////////////////////////////////
// this is the current version of the id_type serialization format
BOOST_CLASS_VERSION(hpx::actions::dynamic_argument, XPI_DYNAMIC_ARGUMENT_VERSION)
BOOST_CLASS_TRACKING(hpx::actions::dynamic_argument, boost::serialization::track_never)

BOOST_CLASS_VERSION(hpx::actions::dynamic_argument_impl, XPI_DYNAMIC_ARGUMENT_VERSION)
BOOST_CLASS_TRACKING(hpx::actions::dynamic_argument_impl, boost::serialization::track_never)

#endif
