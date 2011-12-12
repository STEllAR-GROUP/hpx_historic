#if !defined(XPI_COMPONENTS_FUTURE_JULY_21_2010_0202PM)
#define XPI_COMPONENTS_FUTURE_JULY_21_2010_0202PM

#include "hpx/hpx.hpp"
#include "hpx/runtime/components/client_base.hpp"
#include "hpx/runtime/naming/name.hpp"
#include "hpx/runtime/components/stubs/stub_base.hpp"
#include "hpx/lcos/eager_future.hpp"
#include "boost/program_options.hpp"
#include "boost/serialization/export.hpp"
#include "hpx/runtime/components/component_factory.hpp"
#include "xpi/types/gid_list.hpp"
#include "xpi.h"
#include <hpx/runtime/actions/dynamic_plain_action.hpp>
namespace xpi { namespace components { namespace future {
class HPX_COMPONENT_EXPORT server : public hpx::components::detail::managed_component_base<server> {
   hpx::actions::dynamic_argument _arg;
   int result;
   bool isEager;
   hpx::lcos::mutex mtx;
public:
   server()  {}

   enum {
      reg_func_code,
      wait_func_code,
      unreg_func_code,
   } func_codes;

   void reg(hpx::actions::dynamic_argument arg0,bool arg1);
   typedef hpx::actions::action2<
      server, reg_func_code, hpx::actions::dynamic_argument, bool, &server::reg
   > reg_action;

   void wait(xpi::types::gid_list arg0);
   typedef hpx::actions::action1<
      server, wait_func_code, xpi::types::gid_list, &server::wait
   > wait_action;

   void unreg();
   typedef hpx::actions::action0<
      server, unreg_func_code, &server::unreg
   > unreg_action;
};
class stub : public hpx::components::stubs::stub_base<server> {
public:
   static void reg(
      hpx::naming::id_type gid, hpx::actions::dynamic_argument arg0, bool arg1)
   {
      hpx::applier::apply<server::reg_action>(gid,arg0,arg1);
   }
   static void wait(
      hpx::naming::id_type gid, xpi::types::gid_list arg0)
   {
      hpx::applier::apply<server::wait_action>(gid,arg0);
   }
   static void unreg(
      hpx::naming::id_type gid)
   {
      hpx::applier::apply<server::unreg_action>(gid);
   }
};
class client : public hpx::components::client_base<client, stub> {
    typedef hpx::components::client_base<client, stub> base_type;
public:
    client(hpx::naming::id_type gid) : base_type(gid) {}
    client() : base_type() {}

    void reg(hpx::actions::dynamic_argument arg0,bool arg1) {
        BOOST_ASSERT(gid_);
        this->base_type::reg(gid_,arg0,arg1);
    }

    void wait(xpi::types::gid_list arg0) {
        BOOST_ASSERT(gid_);
        this->base_type::wait(gid_,arg0);
    }

    void unreg() {
        BOOST_ASSERT(gid_);
        this->base_type::unreg(gid_);
    }
};
}}}

#endif
