#include "xpi_future.hpp"
#include <xpi.h>
#include <xpi_future.h>
#include <xpi_gid.hpp>
HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<XPI::LCO::future_server> LCO_future_type;




HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(LCO_future_type, XPI_Future);
HPX_REGISTER_ACTION_EX(LCO_future_type::wrapped_type::init_action,action_init);
HPX_REGISTER_ACTION_EX(LCO_future_type::wrapped_type::write_action,action_write);
HPX_REGISTER_ACTION_EX(LCO_future_type::wrapped_type::wait_action,action_wait);
HPX_REGISTER_ACTION_EX(LCO_future_type::wrapped_type::reg_action, action_reg);

HPX_DEFINE_GET_COMPONENT_TYPE(LCO_future_type::wrapped_type);

HPX_REGISTER_ACTION_EX(hpx::lcos::base_lco_with_value<XPI_Buffer_impl>::set_result_action, set_result_action_buffer);
HPX_REGISTER_ACTION_EX(hpx::lcos::base_lco_with_value<XPI_Buffer_impl>::get_value_action, get_value_action_buffer);
HPX_DEFINE_GET_COMPONENT_TYPE_STATIC(hpx::lcos::base_lco_with_value<XPI_Buffer_impl>, hpx::components::component_base_lco_with_value);

struct XPI_LCO_impl
{
    hpx::naming::id_type gid; // just the gid of a component
};

XPI_API_EXPORT XPI_Error XPI_Future_create(XPI_LCO *future, XPI_Action *action, XPI_Buffer *arg, XPI_Gid *gid)
{
    XPI::LCO::future_client cli;

    xpi::action::dynamic_argument dyn_arg(action, arg);
    //hpx::naming::id_type hpx_gid(gid->id_msb_, gid->id_lsb_, hpx::naming::id_type::unmanaged);
    hpx::naming::id_type hpx_gid=gid->impl->gid;
    cli.create(hpx_gid);
    cli.reg(cli.get_gid(), hpx_gid, dyn_arg, true);
    future->impl=new XPI_LCO_impl;
    future->impl->gid=cli.get_gid();
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Future_get(XPI_LCO *future, XPI_Buffer *result)
{
    XPI::LCO::future_client cli(future->impl->gid);
    //cli.create(future->impl->gid);
    *result->impl=cli.wait(future->impl->gid);
    return XPI_SUCCESS;
}

XPI_API_EXPORT XPI_Error XPI_Future_destroy(XPI_LCO *future)
{
    delete future->impl;
    return XPI_SUCCESS;
}
