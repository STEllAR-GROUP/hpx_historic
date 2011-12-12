#include <xpi/components/multicaster.hpp>
#include <xpi/components/multicaster_impl.hpp>

//HPX_REGISTER_COMPONENT_MODULE();
namespace xpi { namespace components { 
typedef hpx::components::managed_component<multicaster::server> multicaster_type;
}}
using xpi::components::multicaster_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(multicaster_type, multicaster);
HPX_REGISTER_ACTION_EX(multicaster_type::wrapped_type::init_action,action_init);
HPX_REGISTER_ACTION_EX(multicaster_type::wrapped_type::fire_action,action_fire);
HPX_DEFINE_GET_COMPONENT_TYPE(multicaster_type::wrapped_type);

