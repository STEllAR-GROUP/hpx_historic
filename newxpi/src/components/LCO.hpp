#ifndef LCO_H
#define LCO_H 1
#include "hpx/hpx.hpp"
#include "hpx/runtime/components/client_base.hpp"
#include "hpx/runtime/naming/name.hpp"
#include "hpx/runtime/components/stubs/stub_base.hpp"
#include "hpx/lcos/eager_future.hpp"
#include "boost/program_options.hpp"
#include "boost/serialization/export.hpp"
#include "hpx/runtime/components/component_factory.hpp"
#include "xpi.h"
#include "xpi_buffer.hpp"


namespace XPI { namespace LCO {
class HPX_COMPONENT_EXPORT base_lco : public hpx::components::detail::managed_component_base<base_lco> 
{

public:
   base_lco()  {}
   virtual ~base_lco() {}
   enum {
      init_func_code,
      write_func_code,
      wait_func_code,

   } func_codes;
   typedef base_lco base_type_holder;

   virtual void init() {}
   void _init() { init(); }
   typedef hpx::actions::action0<base_lco, init_func_code , &base_lco::_init> init_action;

   virtual void write(XPI_Buffer_impl buf) {}
   void _write(XPI_Buffer_impl buf) { write(buf); }
   typedef hpx::actions::action1<base_lco, write_func_code , XPI_Buffer_impl, &base_lco::_write> write_action;

   virtual XPI_Buffer_impl wait(hpx::naming::id_type gid) { return XPI_Buffer_impl(); }
   XPI_Buffer_impl _wait(hpx::naming::id_type gid) { return wait(gid); }
   typedef hpx::actions::result_action1<base_lco, XPI_Buffer_impl, wait_func_code, hpx::naming::id_type, &base_lco::_wait> wait_action;
};

} }
#endif
