#include <xpi/components/future.hpp>
#include <xpi/components/future_impl.hpp>
HPX_REGISTER_COMPONENT_MODULE();
namespace xpi { namespace components { 
typedef hpx::components::managed_component<future::server> future_type;
}}
using xpi::components::future_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(future_type, future);
HPX_REGISTER_ACTION_EX(future_type::wrapped_type::reg_action,action_reg);
HPX_REGISTER_ACTION_EX(future_type::wrapped_type::wait_action,action_wait);
HPX_REGISTER_ACTION_EX(future_type::wrapped_type::unreg_action,action_unreg);
HPX_DEFINE_GET_COMPONENT_TYPE(future_type::wrapped_type);

