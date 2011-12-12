#include <xpi/components/mutex.hpp>
#include <xpi/components/mutex_impl.hpp>
HPX_REGISTER_COMPONENT_MODULE();
namespace xpi { namespace components { 
typedef hpx::components::managed_component<mutex::server> mutex_type;
}}
using xpi::components::mutex_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(mutex_type, mutex);
HPX_REGISTER_ACTION_EX(mutex_type::wrapped_type::reg_action,action_reg);
HPX_REGISTER_ACTION_EX(mutex_type::wrapped_type::wait_action,action_wait);
HPX_REGISTER_ACTION_EX(mutex_type::wrapped_type::unreg_action,action_unreg);
HPX_DEFINE_GET_COMPONENT_TYPE(mutex_type::wrapped_type);

