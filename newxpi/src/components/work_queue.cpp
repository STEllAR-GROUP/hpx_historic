#include "work_queue.hpp"
#include "work_queue_impl.hpp"
HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<work_queue::server> work_queue_type;




HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(work_queue_type, work_queue);
HPX_REGISTER_ACTION_EX(work_queue_type::wrapped_type::init_action,action_init);
HPX_REGISTER_ACTION_EX(work_queue_type::wrapped_type::wait_action,action_wait);

HPX_DEFINE_GET_COMPONENT_TYPE(work_queue_type::wrapped_type);

