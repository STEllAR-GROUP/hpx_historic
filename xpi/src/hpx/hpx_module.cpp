//  Copyright (c) 2007-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/runtime/components/component_factory.hpp>
#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <hpx/runtime/actions/dynamic_plain_action.hpp>
#include <hpx/runtime/components/server/dynamic_component.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>

///////////////////////////////////////////////////////////////////////////////
// Add factory registration functionality
HPX_REGISTER_COMPONENT_MODULE();

///////////////////////////////////////////////////////////////////////////////
typedef hpx::components::server::dynamic_component dynamic_component_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::simple_component<dynamic_component_type>, 
    dynamic_component);

///////////////////////////////////////////////////////////////////////////////
// Serialization support for the dynamic_component actions
HPX_REGISTER_ACTION_EX(
    dynamic_component_type::dynamic_component_action,
    dynamic_component_dynamic_action);
HPX_DEFINE_GET_COMPONENT_TYPE(dynamic_component_type);

HPX_REGISTER_ACTION_EX(
    hpx::actions::dynamic_plain_action,
    dynamic_plain_action);

///////////////////////////////////////////////////////////////////////////////
typedef hpx::lcos::base_lco_with_value<
        hpx::actions::dynamic_argument 
    > dynamic_action_result_type;

HPX_REGISTER_ACTION_EX(
    dynamic_action_result_type::set_result_action,
    set_result_action_dynamic_action_result);
HPX_DEFINE_GET_COMPONENT_TYPE(dynamic_action_result_type);



