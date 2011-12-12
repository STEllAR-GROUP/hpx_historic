#include "xpi_mutex.hpp"
#include <xpi.h>
#include <xpi_gid.hpp>
HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<XPI::LCO::mutex_server> LCO_mutex_type;




HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(LCO_mutex_type, XPI_Mutex);
HPX_REGISTER_ACTION_EX(LCO_mutex_type::wrapped_type::init_action,action_init);
HPX_REGISTER_ACTION_EX(LCO_mutex_type::wrapped_type::write_action,action_write);
HPX_REGISTER_ACTION_EX(LCO_mutex_type::wrapped_type::wait_action,action_wait);
//HPX_REGISTER_ACTION_EX(LCO_future_type::wrapped_type::reg_action, action_reg);

HPX_DEFINE_GET_COMPONENT_TYPE(LCO_mutex_type::wrapped_type);

HPX_REGISTER_ACTION_EX(hpx::lcos::base_lco_with_value<XPI_Buffer_impl>::set_result_action, set_result_action_buffer);
HPX_REGISTER_ACTION_EX(hpx::lcos::base_lco_with_value<XPI_Buffer_impl>::get_value_action, get_value_action_buffer);
HPX_DEFINE_GET_COMPONENT_TYPE_STATIC(hpx::lcos::base_lco_with_value<XPI_Buffer_impl>, hpx::components::component_base_lco_with_value);

struct XPI_LCO_impl
{
    hpx::naming::id_type gid; // just the gid of a component
};

XPI_API_EXPORT XPI_Error XPI_Mutex_create(XPI_LCO *mutex, XPI_Gid *gid)
{
    XPI::LCO::mutex_client cli;

    hpx::naming::id_type hpx_gid=gid->impl->gid;
    cli.create(hpx_gid);
    cli.init();
    mutex->impl=new XPI_LCO_impl;
    mutex->impl->gid=hpx_gid;
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Mutex_lock(XPI_LCO *mutex)
{
    XPI::LCO::mutex_client cli;
    cli.create(mutex->impl->gid);
    cli.wait(mutex->impl->gid);
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Mutex_unlock(XPI_LCO *mutex)
{
    XPI::LCO::mutex_client cli;
    cli.create(mutex->impl->gid);
    XPI_Buffer_impl throwaway;
    cli.write(throwaway);
    delete mutex->impl;
    return XPI_SUCCESS;
}
