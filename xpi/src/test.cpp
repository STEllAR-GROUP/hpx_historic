/*
 * Copyright (c) 2009 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <xpi.h>
#include <stdio.h>
#include <hpx/hpx.hpp>
#include <hpx/runtime/actions/dynamic_argument.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

XPI_Error xpi_main(int argc, char *argv[])
{    
    hpx::applier::applier& appl = hpx::applier::get_applier();
    
    std::vector<hpx::naming::gid_type> prefixes;
    hpx::naming::gid_type this_prefix = appl.get_runtime_support_raw_gid();
    hpx::naming::gid_type that_prefix;
    if (appl.get_remote_prefixes(prefixes)) {
        that_prefix = prefixes[0];
    }
    else {
        that_prefix = this_prefix;
    }
    std::cout << "this prefix: " << this_prefix << "\nthat prefix: " << that_prefix << "\n";

    XPI_Gid *gid=new XPI_Gid[1];
    gid[0].id_msb_=that_prefix.get_msb();
    gid[0].id_lsb_=that_prefix.get_lsb();
    
    /*
     XPI_Arg {
      XPI_Array XPI_Struct[1] {
        XPI_I16
      }
      XPI_U8
     }
     */
    /*XPI_Type *arg_types=new XPI_Type[6];
    arg_types[0]=XPI_Array_Type;   
    arg_types[1]=1;
    arg_types[2]=XPI_Struct_Type;
    arg_types[3]=1;
    arg_types[4]=XPI_I16_Type;
    arg_types[5]=XPI_U8_Type;*/

    XPI_Type *arg_types=new XPI_Type[2];
    arg_types[0]=XPI_U64_Type;
    arg_types[1]=XPI_U64_Type;
    XPI_Action action={0};

    char *function_name=new char[8];
    strcpy(function_name, "foo.bar");
    XPI_Action_create(function_name, 2, 6, arg_types, &action);
    
    /*XPI_Struct *arg=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct)*2);
    typedef XPI_Struct* XPI_Struct_array;
    arg[0].array.str=(XPI_Struct_array *)XPI_alloc(sizeof(XPI_Struct_array));
    arg[0].array.str[0]=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct));
    arg[0].array.str[0][0].i16=-2;
    arg[1].u8=254;*/

    XPI_Struct *arg=(XPI_Struct *)XPI_alloc(sizeof(XPI_Struct)*2);
    arg[0].u64=gid->id_msb_;
    arg[1].u64=gid->id_lsb_;
    
    XPI_Attrs attrs = { 0 };
    //if (argc==1)
        XPI_Apply(attrs, &action, arg, 1, gid, 0, NULL, 0, NULL);
    
    /*hpx::actions::dynamic_argument dynamic_arg(action, arg);
    std::ofstream ofs("/Users/nalex/test.out");
    boost::archive::text_oarchive oa(ofs);    
    oa << dynamic_arg;*/
    
    /*hpx::actions::dynamic_argument dynamic_arg2;
    std::ifstream ifs("/Users/nalex/test.out");
    boost::archive::text_iarchive ia(ifs);
    ia >> dynamic_arg2;
    hpx::actions::dynamic_argument_impl da=*dynamic_arg2.ptr;*/
    
    /*std::ofstream ofs2("/Users/nalex/test2.xml");
    boost::archive::text_oarchive oa2(ofs2);    
    oa2 << dynamic_arg;*/
    //std::cout << dynamic_arg;
    
    return XPI_SUCCESS;
}

int main(int argc, char *argv[])
{
    XPI_Exec(&argc, &argv, xpi_main);
    return 0;
}
