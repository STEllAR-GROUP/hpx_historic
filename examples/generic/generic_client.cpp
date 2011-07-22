//  Copyright (c) 2007-2010 Hartmut Kaiser, Richard D Guidry Jr
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <iostream>

#include <hpx/hpx.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "generic/generic_component_noresult.hpp"
#include "generic/generic_component_result.hpp"

namespace po = boost::program_options;

using namespace hpx;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
int hpx_main()
{
    // get list of all known localities
    std::vector<naming::id_type> prefixes;
    applier::applier& appl = applier::get_applier();
    naming::id_type prefix;
    if (appl.get_remote_prefixes(prefixes)) {
        // create accumulator on any of the remote localities
        prefix = prefixes[0];
    }
    else {
        // create an accumulator locally
        prefix = appl.get_runtime_support_gid();
    }

    {
        lcos::eager_future<generate_number_action> gen(prefix);
        applier::apply<print_number_action>(prefix, gen.get());
    }

    // initiate shutdown of the runtime systems on all localities
    components::stubs::runtime_support::shutdown_all();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
bool parse_commandline(int argc, char *argv[], po::variables_map& vm)
{
    try {
        po::options_description desc_cmdline ("Usage: generic_client [options]");
        desc_cmdline.add_options()
            ("help,h", "print out program usage (this message)")
            ("run_agas_server,r", "run AGAS server as part of this runtime instance")
            ("worker,w", "run this instance in worker (non-console) mode")
            ("agas,a", po::value<std::string>(), 
                "the IP address the AGAS server is running on (default taken "
                "from hpx.ini), expected format: 192.168.1.1:7912")
            ("hpx,x", po::value<std::string>(), 
                "the IP address the HPX parcelport is listening on (default "
                "is localhost:7910), expected format: 192.168.1.1:7913")
            ("threads,t", po::value<int>(), 
                "the number of operating system threads to spawn for this "
                "HPX locality")
        ;

        po::store(po::command_line_parser(argc, argv)
            .options(desc_cmdline).run(), vm);
        po::notify(vm);

        // print help screen
        if (vm.count("help")) {
            std::cout << desc_cmdline;
            return false;
        }
    }
    catch (std::exception const& e) {
        std::cerr << "generic_client: exception caught: " << e.what() << std::endl;
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
inline void 
split_ip_address(std::string const& v, std::string& addr, boost::uint16_t& port)
{
    std::string::size_type p = v.find_first_of(":");
    try {
        if (p != std::string::npos) {
            addr = v.substr(0, p);
            port = boost::lexical_cast<boost::uint16_t>(v.substr(p+1));
        }
        else {
            addr = v;
        }
    }
    catch (boost::bad_lexical_cast const& /*e*/) {
        std::cerr << "generic_client: illegal port number given: " << v.substr(p+1) << std::endl;
        std::cerr << "                using default value instead: " << port << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
// helper class for AGAS server initialization
class agas_server_helper
{
public:
    agas_server_helper(std::string host, boost::uint16_t port)
      : agas_pool_(), agas_(agas_pool_, host, port)
    {
        agas_.run(false);
    }

private:
    hpx::util::io_service_pool agas_pool_; 
    hpx::naming::resolver_server agas_;
};

///////////////////////////////////////////////////////////////////////////////
// this is the runtime type we use in this application
typedef hpx::runtime_impl<hpx::threads::policies::global_queue_scheduler> runtime_type;

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    try {
        // analyze the command line
        po::variables_map vm;
        if (!parse_commandline(argc, argv, vm))
            return -1;

        // Check command line arguments.
        std::string hpx_host(HPX_INITIAL_IP_ADDRESS), agas_host;
        boost::uint16_t hpx_port = HPX_INITIAL_IP_PORT, agas_port = 0;
        int num_threads = 1;
        hpx::runtime_mode mode = hpx::runtime_mode_console;    // default is console mode

        // extract IP address/port arguments
        if (vm.count("agas")) 
            split_ip_address(vm["agas"].as<std::string>(), agas_host, agas_port);

        if (vm.count("hpx")) 
            split_ip_address(vm["hpx"].as<std::string>(), hpx_host, hpx_port);

        if (vm.count("threads"))
            num_threads = vm["threads"].as<int>();

        if (vm.count("worker"))
            mode = hpx::runtime_mode_worker;

        // initialize and run the AGAS service, if appropriate
        std::auto_ptr<agas_server_helper> agas_server;
        if (vm.count("run_agas_server"))  // run the AGAS server instance here
            agas_server.reset(new agas_server_helper(agas_host, agas_port));

        // initialize and start the HPX runtime
        runtime_type rt(hpx_host, hpx_port, agas_host, agas_port, mode);
        rt.run(hpx_main, num_threads);
    }
    catch (std::exception& e) {
        std::cerr << "std::exception caught: " << e.what() << "\n";
        return -1;
    }
    catch (...) {
        std::cerr << "unexpected exception caught\n";
        return -2;
    }
    return 0;
}

