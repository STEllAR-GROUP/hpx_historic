#include "LCO.hpp"
HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<XPI::LCO::base_lco> LCO_type;




HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(LCO_type, LCO);
HPX_REGISTER_ACTION_EX(LCO_type::wrapped_type::init_action,action_init);
HPX_REGISTER_ACTION_EX(LCO_type::wrapped_type::write_action,action_write);
HPX_REGISTER_ACTION_EX(LCO_type::wrapped_type::wait_action,action_wait);

HPX_DEFINE_GET_COMPONENT_TYPE(LCO_type::wrapped_type);

