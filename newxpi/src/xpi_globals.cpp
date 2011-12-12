/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#include <xpi.h>

#include <iostream>
#include <memory>
#include <string>
#include <hpx/hpx.hpp>
#include <hpx/runtime/actions/plain_action.hpp>
#include <boost/atomic.hpp>
#include <xpi_gid.hpp>

using namespace hpx;

inline void split_ip_address(std::string const& v, std::string& addr, boost::uint16_t& port)
{
    std::string::size_type p = v.find_first_of(":");
    try
    {   
        if (p != std::string::npos)
        {   
            addr = v.substr(0, p);
            port = boost::lexical_cast<boost::uint16_t>(v.substr(p+1));
        }
        else
        {   
            addr = v;
        }
    }
    catch (boost::bad_lexical_cast const& /*e*/)
    {   
        std::cerr << "XPI: illegal port number given: " << v.substr(p+1) << std::endl;
        std::cerr << "           using default value instead: " << port << std::endl;
    }
}

class agas_server_helper
{
public:
    agas_server_helper(std::string host, boost::uint16_t port)
    : agas_pool_(), agas_(agas_pool_, host, port)
    {   
        agas_.run(false);
    }
    ~agas_server_helper()
    {   
        agas_.stop();
    }

private:
    hpx::util::io_service_pool agas_pool_;
    hpx::naming::resolver_server agas_;
};

XPI_API_EXPORT XPI_Error XPI_Runtime_gid(XPI_Gid *gid)
{
    hpx::applier::applier& appl = hpx::applier::get_applier();
    hpx::naming::id_type hpx_gid = appl.get_runtime_support_gid();
    gid->impl=new XPI_Gid_impl;
    gid->impl->gid=hpx_gid;
    //gid->id_msb_=hpx_gid.get_msb();
    //gid->id_lsb_=hpx_gid.get_lsb();
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Shutdown()
{
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Init(int *argc, char **argv[], XPI_Error (*xpi_main)(int, char *[]))
{
    return XPI_SUCCESS;
}

XPI_Error _XPI_Main(int argc, char *argv[], XPI_Error (*xpi_main)(int, char*[]))
{   
    XPI_Error retcode=xpi_main(argc, argv);
    components::stubs::runtime_support::shutdown_all();
    return retcode;
}


XPI_API_EXPORT XPI_Error XPI_Exec(int *argc, char **argv[], XPI_Error (*xpi_main)(int, char *[]))
{
    std::string hpx_host("localhost"), agas_host("localhost");
    boost::uint16_t hpx_port = HPX_PORT, agas_port = 0;
    int num_threads = 1;
    hpx::runtime_mode mode = hpx::runtime_mode_console;    // default is console mode
    int run_agas_server=0;
    char **new_argv=*argv;

    // Parse arguments
    int i;
    int remaining_argc=1;

    for (i=1; i<*argc; i++)
    {
        std::string arg(new_argv[i]);
        if ((arg=="--agas" || arg=="-a") && i+1<*argc)
            split_ip_address(std::string(new_argv[++i]), agas_host, agas_port);
        else if (arg=="--hpx" || arg=="-x" && i+1<*argc)
            split_ip_address(std::string(new_argv[++i]), hpx_host, hpx_port);
        else if (arg=="--threads" || arg=="-t" && i+1<*argc)
            num_threads=atoi(new_argv[++i]);
        else if (arg=="--worker" || arg=="-w")
            mode=hpx::runtime_mode_worker;
        else if (arg=="--run_agas_server" || arg=="-r")
            run_agas_server=1;
        else if (arg=="--help" || arg=="-h")
            ;
        else
        {  
           new_argv[remaining_argc++]=new_argv[i];
        }
    }
    
    *argc=remaining_argc;
    try {
        std::auto_ptr<agas_server_helper> agas_server;
        if (run_agas_server)
            agas_server.reset(new agas_server_helper(agas_host, agas_port));
        
        typedef hpx::runtime_impl<hpx::threads::policies::global_queue_scheduler> runtime_type;
         
        runtime_type rt(hpx_host, hpx_port, agas_host, agas_port, mode);
        rt.run(boost::bind(_XPI_Main, remaining_argc, new_argv, xpi_main), num_threads);
    }
    catch (...)
    {   
        return XPI_ERROR;
    }
    return XPI_SUCCESS;
}
