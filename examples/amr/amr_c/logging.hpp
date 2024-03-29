//  Copyright (c) 2007-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_COMPONENTS_AMR_LOGGING_NOV_10_2008_0719PM)
#define HPX_COMPONENTS_AMR_LOGGING_NOV_10_2008_0719PM

#include <hpx/lcos/mutex.hpp>
#include "stencil_data.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace amr { namespace server
{
    /// This class implements a simple logging sink. It exposes the function
    /// \a logentry which is supposed to record the received values in a 
    /// application specific manner.
    class HPX_COMPONENT_EXPORT logging 
      : public simple_component_base<logging>
    {
    private:
        typedef simple_component_base<logging> base_type;

    public:
        enum actions
        {
            logging_logentry = 0,
        };

        /// This is the function implementing the logging functionality
        /// It takes the values as calculated during the current time step.
        void logentry(stencil_data const& memblock_gid, int row);

        /// Each of the exposed functions needs to be encapsulated into an action
        /// type, allowing to generate all required boilerplate code for threads,
        /// serialization, etc.
        ///
        /// The \a set_result_action may be used to trigger any LCO instances
        /// while carrying an additional parameter of any type.
        ///
        /// \param Result [in] The type of the result to be transferred back to 
        ///               this LCO instance.
        typedef hpx::actions::action2<
            logging, logging_logentry, stencil_data const&, int,
            &logging::logentry
        > logentry_action;

    private:
        typedef lcos::mutex mutex_type;
        mutex_type mtx_;
    };

}}}}

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace amr { namespace stubs 
{
    ///////////////////////////////////////////////////////////////////////////
    struct logging : public components::stubs::stub_base<amr::server::logging>
    {
        ///////////////////////////////////////////////////////////////////////
        static void logentry(naming::id_type const& gid, 
            stencil_data const& val, int row)
        {
            typedef amr::server::logging::logentry_action action_type;
            applier::apply<action_type>(gid, val, row);
        }
    };

}}}}

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace amr 
{
    ///////////////////////////////////////////////////////////////////////////
    class logging : public client_base<logging, amr::stubs::logging>
    {
    private:
        typedef client_base<logging, amr::stubs::logging> base_type;

    public:
        logging(naming::id_type gid, bool freeonexit = false)
          : base_type(gid, freeonexit)
        {}

        ///////////////////////////////////////////////////////////////////////
        void logentry(stencil_data const& val, int row)
        {
            this->base_type::logentry(this->gid_, val, row);
        }
    };

}}}

#endif
