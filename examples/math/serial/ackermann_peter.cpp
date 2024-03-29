////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

boost::uint64_t ackermann_peter(boost::uint64_t m, boost::uint64_t n)
{
    if (0 == m)
        return n + 1;
    else
    {
        if (0 == n)
            return ackermann_peter(m - 1, 1);
        else
            return ackermann_peter(m - 1, ackermann_peter(m, n - 1));
    } 
}

int main(int argc, char** argv)
{
    try
    {
        if (3 != argc)
            throw std::exception();

        const boost::uint64_t m = boost::lexical_cast<boost::uint64_t>(argv[1]);
        const boost::uint64_t n = boost::lexical_cast<boost::uint64_t>(argv[2]);

        std::cout
            << ( boost::format("ackermann_peter(%1%, %2%) == %3%\n")
               % m % n % ackermann_peter(m, n));
    }

    catch (std::exception&)
    {
        std::cerr << (boost::format("Usage: %1% M N\n") % argv[0]);
        return 1;
    }  
}

