#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H 1
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
#include "hpx/runtime/threads/threadmanager.hpp"


namespace work_queue { 
class HPX_COMPONENT_EXPORT server : public hpx::components::detail::managed_component_base<server>  {
   XPI_Buffer_impl receptor;

public:
   server()  {}
   enum {
      init_func_code,
      wait_func_code,

   } func_codes;

   void init(XPI_Buffer_impl );
   typedef hpx::actions::action1<
      server, init_func_code , XPI_Buffer_impl, &server::init
   > init_action;

   void wait(hpx::naming::id_type );
   typedef hpx::actions::action1<
      server, wait_func_code , hpx::naming::id_type, &server::wait
   > wait_action;

};

class stub : public hpx::components::stubs::stub_base<server> {
public:

   static void init(
   	hpx::naming::id_type gid, XPI_Buffer_impl arg0)
   {
      hpx::applier::apply<server::init_action>(gid, arg0);
   }

   static void wait(
   	hpx::naming::id_type gid, hpx::naming::id_type arg0)
   {
      hpx::applier::apply<server::wait_action>(gid, arg0);
   }

};

class client : public hpx::components::client_base<client, stub> {
   typedef hpx::components::client_base<client, work_queue::stub> base_type;
public:
   client(hpx::naming::id_type gid) : base_type(gid) {}
   client() : base_type() {}

   void init(XPI_Buffer_impl arg0) {
      BOOST_ASSERT(gid_);
      this->base_type::init(gid_, arg0);
   }

   void wait(hpx::naming::id_type arg0) {
      BOOST_ASSERT(gid_);
      this->base_type::wait(gid_, arg0);
   }

};
}
#endif
