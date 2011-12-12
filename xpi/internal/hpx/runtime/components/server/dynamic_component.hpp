//  Copyright (c) 2007-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(XPI_HPX_RUNTIME_ACTIONS_DYNAMIC_COMPONENT_ACTION_OCT_28_2009_1058AM)
#define XPI_HPX_RUNTIME_ACTIONS_DYNAMIC_COMPONENT_ACTION_OCT_28_2009_1058AM

#include <hpx/hpx_fwd.hpp>
#include <hpx/config.hpp>

#include <hpx/runtime/actions/component_action.hpp>
#include <hpx/runtime/actions/dynamic_argument.hpp>

#include <vector>

#include <hpx/config/warnings_prefix.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace server
{
    ///////////////////////////////////////////////////////////////////////////
    class HPX_COMPONENT_EXPORT dynamic_component
      : public simple_component_base<dynamic_component>
    {
    private:
        enum { dynamic_action_code = 1 };

    public:
        actions::dynamic_argument dynamic_action_wrapper(
          std::vector<actions::dynamic_argument> const&);

        typedef actions::result_action1<
            dynamic_component, actions::dynamic_argument, dynamic_action_code, 
            std::vector<actions::dynamic_argument> const&,
            &dynamic_component::dynamic_action_wrapper
        > dynamic_component_action;
    };
}}}

#include <hpx/config/warnings_suffix.hpp>

#endif
