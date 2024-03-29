////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

// Naive SMP version implemented with eager_futures and arbitrary size integers.

#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/bigint.hpp>
#include <boost/bigint/serialize.hpp>
#include <boost/format.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/runtime/actions/plain_action.hpp>
#include <hpx/runtime/components/plain_component_factory.hpp>
#include <hpx/util/high_resolution_timer.hpp>
#include <hpx/lcos/eager_future.hpp>

using boost::program_options::variables_map;
using boost::program_options::options_description;
using boost::program_options::value;

using boost::bigint;

using hpx::naming::id_type;

using hpx::applier::get_applier;

using hpx::actions::plain_result_action2;

using hpx::lcos::eager_future;

using hpx::util::high_resolution_timer;

using hpx::init;
using hpx::finalize;

///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<bigint> fibonacci(
    id_type const& prefix 
  , boost::uint64_t m
);

typedef plain_result_action2<
    // result type
    boost::shared_ptr<bigint> 
    // arguments
  , id_type const&  
  , boost::uint64_t
    // function
  , fibonacci
> fibonacci_action;

HPX_REGISTER_PLAIN_ACTION(fibonacci_action);

typedef eager_future<fibonacci_action> fibonacci_future;

///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<bigint> fibonacci(
    id_type const& prefix
  , boost::uint64_t n
) {
    if (2 > n)
        return boost::make_shared<bigint>(n);

    fibonacci_future n1(prefix, prefix, n - 1); 
    fibonacci_future n2(prefix, prefix, n - 2); 

    boost::shared_ptr<bigint> r = n1.get();
    *r += *n2.get();
    return r;
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(variables_map& vm)
{
    {
        boost::uint64_t n = vm["n-value"].as<boost::uint64_t>();

        const id_type prefix = get_applier().get_runtime_support_gid();

        high_resolution_timer t;

        fibonacci_future f(prefix, prefix, n);

        boost::shared_ptr<bigint> r = f.get();

        double elapsed = t.elapsed();

        std::cout
            << ( boost::format("fibonacci(%1%) == %2%\n"
                               "elapsed time == %3%\n")
               % n % *r % elapsed);
    }

    finalize();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Configure application-specific options
    options_description
       desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        ( "n-value"
        , value<boost::uint64_t>()->default_value(10)
        , "n value for the fibonacci function") 
        ;

    // Initialize and run HPX
    return init(desc_commandline, argc, argv);
}

