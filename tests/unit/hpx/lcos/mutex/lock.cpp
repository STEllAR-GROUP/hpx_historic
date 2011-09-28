//  Copyright (C) 2001-2003 William E. Kempf
//  Copyright (C) 2007-2011 Hartmut Kaiser
//  Copyright (C)      2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// TODO: Test unprotected case, use functions instead of function objects/
// reference data members.

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/util/lightweight_test.hpp>
#include <hpx/lcos/mutex.hpp>
#include <hpx/lcos/barrier.hpp>

using boost::program_options::variables_map;
using boost::program_options::options_description;
using boost::program_options::value;

using hpx::naming::id_type;

using hpx::applier::get_applier;
using hpx::applier::register_work_nullary;

using hpx::lcos::barrier;
using hpx::lcos::mutex;

using hpx::init;
using hpx::finalize;

using hpx::util::report_errors;

///////////////////////////////////////////////////////////////////////////////
struct test_unprotected_data
{
    std::size_t data;

    test_unprotected_data() : data(0) {}

    void operator()() { ++data; }
};

template <typename M>
struct test_mutexed_data
{
    typedef M mutex_type;
    typedef typename M::scoped_lock lock_type;

    mutex_type& mtx;
    barrier& barr;
    std::size_t& data;

    test_mutexed_data(mutex_type& m, barrier& b, std::size_t& d)
        : mtx(m), barr(b), data(d) {}

    void operator()()
    {
        lock_type lock(mtx);
        HPX_TEST(lock ? true : false);

        ++data;
 
        lock.unlock();
        HPX_TEST(!lock);

        barr.wait();
    }
};

///////////////////////////////////////////////////////////////////////////////
int hpx_main(variables_map& vm)
{
    std::size_t num_threads = 1;

    if (vm.count("threads"))
        num_threads = vm["threads"].as<std::size_t>();

    std::size_t pxthreads = num_threads * 8;

    if (vm.count("pxthreads"))
        pxthreads = vm["pxthreads"].as<std::size_t>();
    
    id_type prefix = get_applier().get_runtime_support_gid();

    {
        mutex mtx;
        barrier barr;
        std::size_t data = 0;
        barr.create_one(prefix, pxthreads + 1);

        test_mutexed_data<mutex> t(mtx, barr, data);
        for (std::size_t i = 0; i < pxthreads; ++i)
            register_work_nullary(t, "do_test_mutex");

        barr.wait();

        mutex::scoped_lock lock(mtx);
        HPX_TEST(lock ? true : false);

        HPX_TEST_EQ(data, pxthreads);
    } 

    // initiate shutdown of the runtime system
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
        ("pxthreads,T", value<std::size_t>(), 
            "the number of PX threads to invoke (default: OS threads * 8)")
        ;

    // Initialize and run HPX
    HPX_TEST_EQ_MSG(init(desc_commandline, argc, argv), 0,
      "HPX main exited with non-zero status");
    return report_errors();
}
