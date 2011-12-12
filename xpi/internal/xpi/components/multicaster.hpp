#if !defined(XPI_COMPONENTS_MULTICASTER_MAR_5_2010_1113AM)
#define XPI_COMPONENTS_MULTICASTER_MAR_5_2010_1113AM

#include "hpx/hpx.hpp"
#include "hpx/runtime/components/client_base.hpp"
#include "hpx/runtime/naming/name.hpp"
#include "hpx/runtime/components/stubs/stub_base.hpp"
#include "hpx/lcos/eager_future.hpp"
#include "boost/program_options.hpp"
#include "boost/serialization/export.hpp"
#include "hpx/runtime/components/component_factory.hpp"
#include "xpi/types/gid_list.hpp"
#include <hpx/runtime/actions/dynamic_plain_action.hpp>

namespace xpi { namespace components { namespace multicaster {
class HPX_COMPONENT_EXPORT server : public hpx::components::detail::managed_component_base<server> {
   xpi::types::gid_list gids;
   hpx::actions::dynamic_argument args;
public:
   server()  {}

   enum {
      init_func_code,
      fire_func_code,
   } func_codes;

   void init(xpi::types::gid_list arg0,hpx::actions::dynamic_argument arg1);
   typedef hpx::actions::action2<
      server, init_func_code, xpi::types::gid_list, hpx::actions::dynamic_argument, &server::init
   > init_action;

   void fire();
   typedef hpx::actions::action0<
      server, fire_func_code, &server::fire
   > fire_action;
};
class stub : public hpx::components::stubs::stub_base<server> {
public:
   static void init(
      hpx::naming::id_type gid, xpi::types::gid_list arg0, hpx::actions::dynamic_argument arg1)
   {
      hpx::applier::apply<server::init_action>(gid,arg0,arg1);
   }
   static void fire(
      hpx::naming::id_type gid)
   {
      hpx::applier::apply<server::fire_action>(gid);
   }
};
class client : public hpx::components::client_base<client, stub> {
    typedef hpx::components::client_base<client, stub> base_type;
public:
    client(hpx::naming::id_type gid) : base_type(gid) {}
    client() : base_type() {}

    void init(xpi::types::gid_list arg0,hpx::actions::dynamic_argument arg1) {
        BOOST_ASSERT(gid_);
        this->base_type::init(gid_,arg0,arg1);
    }

    void fire() {
        BOOST_ASSERT(gid_);
        this->base_type::fire(gid_);
    }
};
}}}

#endif
