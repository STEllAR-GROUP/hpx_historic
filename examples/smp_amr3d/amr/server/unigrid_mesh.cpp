//  Copyright (c) 2007-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/lcos/future_wait.hpp>
#include <hpx/runtime/components/component_factory.hpp>

#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/assign/std/vector.hpp>

#include "../dynamic_stencil_value.hpp"
#include "../functional_component.hpp"
#include "../../parameter.hpp"
#include "../../array3d.hpp"

#include "unigrid_mesh.hpp"

///////////////////////////////////////////////////////////////////////////////
typedef hpx::components::amr::server::unigrid_mesh had_unigrid_mesh_type;

///////////////////////////////////////////////////////////////////////////////
// Serialization support for the actions
HPX_REGISTER_ACTION_EX(had_unigrid_mesh_type::init_execute_action, 
    had_unigrid_mesh_init_execute_action);
HPX_REGISTER_ACTION_EX(had_unigrid_mesh_type::execute_action, 
    had_unigrid_mesh_execute_action);

typedef hpx::lcos::base_lco_with_value<
    boost::shared_ptr<std::vector<hpx::naming::id_type> > > lco_gid_vector_ptr;

HPX_REGISTER_ACTION_EX(lco_gid_vector_ptr::set_result_action,
    set_result_action_gid_vector_ptr);
HPX_DEFINE_GET_COMPONENT_TYPE(lco_gid_vector_ptr);

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::simple_component<had_unigrid_mesh_type>, had_unigrid_mesh3d);
HPX_DEFINE_GET_COMPONENT_TYPE(had_unigrid_mesh_type);

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace amr { namespace server 
{
    unigrid_mesh::unigrid_mesh()
    {}

    inline bool unigrid_mesh::floatcmp(double_type const& x1, double_type const& x2) {
      // compare two floating point numbers
      static double_type const epsilon = 1.e-8;
      if ( x1 + epsilon >= x2 && x1 - epsilon <= x2 ) {
        // the numbers are close enough for coordinate comparison
        return true;
      } else {
        return false;
      }
    }

    inline bool unigrid_mesh::floatcmp_ge(double_type const& x1, double_type const& x2) {
      // compare two floating point numbers
      static double_type const epsilon = 1.e-8;

      if ( x1 > x2 ) return true;

      if ( x1 + epsilon >= x2 && x1 - epsilon <= x2 ) {
        // the numbers are close enough for coordinate comparison
        return true;
      } else {
        return false;
      }
    }

    inline bool unigrid_mesh::floatcmp_le(double_type const& x1, double_type const& x2) {
      // compare two floating point numbers
      static double_type const epsilon = 1.e-8;

      if ( x1 < x2 ) return true;

      if ( x1 + epsilon >= x2 && x1 - epsilon <= x2 ) {
        // the numbers are close enough for coordinate comparison
        return true;
      } else {
        return false;
      }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Initialize functional components by setting the logging component to use
    void unigrid_mesh::init(distributed_iterator_range_type const& functions,
        distributed_iterator_range_type const& logging, std::size_t numsteps)
    {
        components::distributing_factory::iterator_type function = functions.first;
        naming::id_type log = naming::invalid_id;

        if (logging.first != logging.second)
            log = *logging.first;

        typedef std::vector<lcos::future_value<void> > lazyvals_type;
        lazyvals_type lazyvals;

        for (/**/; function != functions.second; ++function)
        {
            lazyvals.push_back(components::amr::stubs::functional_component::
                init_async(*function, numsteps, log));
        }

        wait(lazyvals);   // now wait for the initialization to happen
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Create functional components, one for each data point, use those to 
    // initialize the stencil value instances
    void unigrid_mesh::init_stencils(distributed_iterator_range_type const& stencils,
        distributed_iterator_range_type const& functions, int static_step, 
        array3d &dst_port,array3d &dst_src,array3d &dst_step,
        array3d &dst_size,array3d &src_size, parameter const& par)
    {
        components::distributing_factory::iterator_type stencil = stencils.first;
        components::distributing_factory::iterator_type function = functions.first;

        // start an asynchronous operation for each of the stencil value instances
        typedef std::vector<lcos::future_value<void> > lazyvals_type;
        lazyvals_type lazyvals;

        for (int column = 0; stencil != stencils.second; ++stencil, ++function, ++column)
        {
            namespace stubs = components::amr::stubs;
            BOOST_ASSERT(function != functions.second);

#if 0       // DEBUG
            std::cout << " row " << static_step << " column " << column << " in " << dst_size(static_step,column,0) << " out " << src_size(static_step,column,0) << std::endl;

            // Follow up TEST
            if ( dst_size(static_step,column,0) == 0 ) {
              // figure out the index and level of this point
              std::size_t a,b,c;
              int level = findlevel3D(static_step,column,a,b,c,par);
              std::cout << "                 PROBLEM: level " << level << " index " << a << " " << b << " " << c << std::endl;
            } 
#endif
#if 0
            if ( dst_size(static_step,column,0) > 0 ) {
              std::cout << "                      in row:  " << dst_step(static_step,column,0) << " in column " << dst_src(static_step,column,0) << std::endl;
            }
            if ( dst_size(static_step,column,0) > 1 ) {
              std::cout << "                      in row:  " << dst_step(static_step,column,1) << " in column " << dst_src(static_step,column,1) << std::endl;
            }
            if ( dst_size(static_step,column,0) > 2 ) {
              std::cout << "                      in row:  " << dst_step(static_step,column,2) << " in column " << dst_src(static_step,column,2) << std::endl;
            }
            if ( dst_size(static_step,column,0) > 3 ) {
              std::cout << "                      in row:  " << dst_step(static_step,column,3) << " in column " << dst_src(static_step,column,3) << std::endl;
            }
#endif

            lazyvals.push_back(
                stubs::dynamic_stencil_value::set_functional_component_async(
                    *stencil, *function, static_step, column, 
                    dst_size(static_step, column, 0), 
                    src_size(static_step, column, 0), par));
        }
        //BOOST_ASSERT(function == functions.second);

        wait(lazyvals);   // now wait for the results
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Get gids of output ports of all functions
    void unigrid_mesh::get_output_ports(
        distributed_iterator_range_type const& stencils,
        std::vector<std::vector<naming::id_type> >& outputs)
    {
        typedef components::distributing_factory::result_type result_type;
        typedef 
            std::vector<lcos::future_value<std::vector<naming::id_type> > >
        lazyvals_type;

        // start an asynchronous operation for each of the stencil value instances
        lazyvals_type lazyvals;
        components::distributing_factory::iterator_type stencil = stencils.first;
        for (/**/; stencil != stencils.second; ++stencil)
        {
            lazyvals.push_back(components::amr::stubs::dynamic_stencil_value::
                get_output_ports_async(*stencil));
        }

        wait(lazyvals, outputs);      // now wait for the results
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Connect the given output ports with the correct input ports, creating the 
    // required static data-flow structure.
    //
    // Currently we have exactly one stencil_value instance per data point, where 
    // the output ports of a stencil_value are connected to the input ports of the 
    // direct neighbors of itself.
    void unigrid_mesh::connect_input_ports(
        components::distributing_factory::result_type const* stencils,
        std::vector<std::vector<std::vector<naming::id_type> > > const& outputs,
        array3d &dst_size,array3d &dst_step,array3d &dst_src,array3d &dst_port,
        parameter const& par)
    {
        typedef components::distributing_factory::result_type result_type;
        typedef std::vector<lcos::future_value<void> > lazyvals_type;

        lazyvals_type lazyvals;

        int steps = (int)outputs.size();
        for (int step = 0; step < steps; ++step) 
        {
            components::distributing_factory::iterator_range_type r = 
                locality_results(stencils[step]);

            components::distributing_factory::iterator_type stencil = r.first;
            for (int i = 0; stencil != r.second; ++stencil, ++i)
            {
                std::vector<naming::id_type> output_ports;

                for (int j = 0; j < dst_size(step, i, 0); ++j) {
                    output_ports.push_back(
                        outputs[dst_step(step,i,j)][dst_src(step,i,j)][dst_port(step,i,j)]);
                }

                lazyvals.push_back(components::amr::stubs::dynamic_stencil_value::
                    connect_input_ports_async(*stencil, output_ports));
            }
        }

        wait (lazyvals);      // now wait for the results
    }

    ///////////////////////////////////////////////////////////////////////////////
    void unigrid_mesh::prepare_initial_data(
        distributed_iterator_range_type const& functions, 
        std::vector<naming::id_type>& initial_data,
        std::size_t numvalues,
        parameter const& par)
    {
        typedef std::vector<lcos::future_value<naming::id_type> > lazyvals_type;

        // create a data item value type for each of the functions
        lazyvals_type lazyvals;
        components::distributing_factory::iterator_type function = functions.first;

        for (std::size_t i = 0; function != functions.second; ++function, ++i)
        {
            lazyvals.push_back(components::amr::stubs::functional_component::
                alloc_data_async(*function, i, numvalues, 0,par));
        }

        wait (lazyvals, initial_data);      // now wait for the results
    }

    ///////////////////////////////////////////////////////////////////////////////
    // do actual work
    void unigrid_mesh::execute(
        components::distributing_factory::iterator_range_type const& stencils, 
        std::vector<naming::id_type> const& initial_data, 
        std::vector<naming::id_type>& result_data)
    {
        // start the execution of all stencil stencils (data items)
        typedef std::vector<lcos::future_value<naming::id_type> > lazyvals_type;

        lazyvals_type lazyvals;
        components::distributing_factory::iterator_type stencil = stencils.first;
        for (std::size_t i = 0; stencil != stencils.second; ++stencil, ++i)
        {
            BOOST_ASSERT(i < initial_data.size());
            lazyvals.push_back(components::amr::stubs::dynamic_stencil_value::
                call_async(*stencil, initial_data[i]));
          //  lazyvals.push_back(components::amr::stubs::stencil_value<3>::
          //      call_async(*stencil, initial_data[i]));
        }

        wait (lazyvals, result_data);      // now wait for the results
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // 
    void unigrid_mesh::start_row(
        components::distributing_factory::iterator_range_type const& stencils)
    {
        // start the execution of all stencil stencils (data items)
        typedef std::vector<lcos::future_value<void> > lazyvals_type;

        lazyvals_type lazyvals;
        components::distributing_factory::iterator_type stencil = stencils.first;
        for (std::size_t i = 0; stencil != stencils.second; ++stencil, ++i)
        {
            lazyvals.push_back(components::amr::stubs::dynamic_stencil_value::
                start_async(*stencil));
        }

        wait (lazyvals);      // now wait for the results
    }

    ///////////////////////////////////////////////////////////////////////////
    /// This is the main entry point of this component. 
    boost::shared_ptr<std::vector<naming::id_type> > unigrid_mesh::init_execute(
        components::component_type function_type, std::size_t numvalues, 
        std::size_t numsteps,
        components::component_type logging_type,
        parameter const& par)
    {
        //hpx::util::high_resolution_timer t;
        boost::shared_ptr<std::vector<naming::id_type> > result_data
            (new std::vector<naming::id_type>());

        components::component_type stencil_type = 
            components::get_component_type<components::amr::server::dynamic_stencil_value>();

        typedef components::distributing_factory::result_type result_type;

        // create a distributing factory locally
        components::distributing_factory factory;
        factory.create(applier::get_applier().get_runtime_support_gid());

        // create a couple of stencil (functional) components and twice the 
        // amount of stencil_value components
        result_type functions = factory.create_components(function_type, numvalues);

        int num_rows = par->num_rows;

        // Each row potentially has a different number of points depending on the
        // number of levels of refinement.  There are 2^(nlevel) rows each timestep;
        // this mesh is set up to take two timesteps at a time (i.e. nt0 must be even).
        // The rowsize vector computes how many points are in a row which includes
        // a specified number of levels.  For example, rowsize[0] indicates the total
        // number of points in a row that includes all levels down to the coarsest, level=0; 
        // similarly, rowsize[1] indicates the total number of points in a row that includes
        // all finer levels down to level=1.

        // vectors level_begin and level_end indicate the level to which a particular index belongs

        // The vector each_row connects the rowsize vector with each of the 2^nlevel rows.
        // The pattern is as follows: 
        //     For 0 levels of refinement--
        //       there are 2 rows:    
        //                            1  rowsize[0]
        //                            0  rowsize[0]
        //  
        //     For 1 level of refinement--
        //       there are 4 rows:  
        //                            3  rowsize[1]
        //                            2  rowsize[0]
        //                            1  rowsize[1]
        //                            0  rowsize[0]
        // 
        //     For 2 levels of refinement--
        //       there are 8 rows: 
        //                             7 rowsize[2]
        //                             6 rowsize[1]
        //                             5 rowsize[2]
        //                             4 rowsize[0]
        //                             3 rowsize[2]
        //                             2 rowsize[1]
        //                             1 rowsize[2]
        //                             0 rowsize[0]
        //
        //  This structure is designed so that every 2 rows the timestep is the same for every point in that row

        std::vector<std::size_t> each_row;
        each_row.resize(num_rows);
        for (int i=0;i<num_rows;i++) {
          each_row[i] = par->rowsize[par->level_row[i]];
        }

        std::vector<result_type> stencils;
        for (int i=0;i<num_rows;i++) {
          stencils.push_back(factory.create_components(stencil_type, each_row[i]));
        }

        // initialize logging functionality in functions
        result_type logging;
        if (logging_type != components::component_invalid)
            logging = factory.create_components(logging_type);

        init(locality_results(functions), locality_results(logging), numsteps);

        // prep the connections
        std::size_t memsize = 28;
        array3d dst_port(num_rows,each_row[0],memsize);
        array3d dst_src(num_rows,each_row[0],memsize);
        array3d dst_step(num_rows,each_row[0],memsize);
        array3d dst_size(num_rows,each_row[0],1);
        array3d src_size(num_rows,each_row[0],1);
        prep_ports(dst_port,dst_src,dst_step,dst_size,src_size,
                   num_rows,each_row,par);

        // initialize stencil_values using the stencil (functional) components
        for (int i = 0; i < num_rows; ++i) 
            init_stencils(locality_results(stencils[i]), locality_results(functions), i,
                          dst_port,dst_src,dst_step,dst_size,src_size, par);

        // ask stencil instances for their output gids
        std::vector<std::vector<std::vector<naming::id_type> > > outputs(num_rows);
        for (int i = 0; i < num_rows; ++i) 
            get_output_ports(locality_results(stencils[i]), outputs[i]);

        // connect output gids with corresponding stencil inputs
        connect_input_ports(&*stencils.begin(), outputs,dst_size,dst_step,dst_src,dst_port,par);

        // for loop over second row ; call start for each
        for (int i = 1; i < num_rows; ++i) 
            start_row(locality_results(stencils[i]));

        // prepare initial data
        std::vector<naming::id_type> initial_data;
        prepare_initial_data(locality_results(functions), initial_data, 
                             each_row[0],par);

        //std::cout << " Startup grid cost " << t.elapsed() << std::endl;
        // do actual work
        execute(locality_results(stencils[0]), initial_data, *result_data);

        // free all allocated components (we can do that synchronously)
        if (!logging.empty())
            factory.free_components_sync(logging);

        for (int i = 0; i < num_rows; ++i) 
            factory.free_components_sync(stencils[i]);
        factory.free_components_sync(functions);

        return result_data;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// This the other entry point of this component. 
    std::vector<naming::id_type> unigrid_mesh::execute(
        std::vector<naming::id_type> const& initial_data,
        components::component_type function_type, std::size_t numvalues, 
        std::size_t numsteps,
        components::component_type logging_type, parameter const& par)
    {
        BOOST_ASSERT(false);
        // This routine is deprecated currently

        std::vector<naming::id_type> result_data;
#if 0
        components::component_type stencil_type = 
            components::get_component_type<components::amr::server::dynamic_stencil_value >();

        typedef components::distributing_factory::result_type result_type;

        // create a distributing factory locally
        components::distributing_factory factory;
        factory.create(applier::get_applier().get_runtime_support_gid());

        // create a couple of stencil (functional) components and twice the 
        // amount of stencil_value components
        result_type functions = factory.create_components(function_type, numvalues);

        std::vector<result_type> stencils;

        double_type tmp = 3*pow(2,par->allowedl);
        int num_rows = (int) tmp; 

        std::vector<std::size_t> rowsize;
        for (int j=0;j<=par->allowedl;j++) {
          rowsize.push_back(par->nx[par->allowedl]);
          for (int i=par->allowedl-1;i>=j;i--) {
            // remove duplicates
            rowsize[j] += par->nx[i] - (par->nx[i+1]+1)/2;
          }
        }

        for (int i=0;i<num_rows;i++) stencils.push_back(factory.create_components(stencil_type, numvalues));

        // initialize logging functionality in functions
        result_type logging;
        if (logging_type != components::component_invalid)
            logging = factory.create_components(logging_type);

        init(locality_results(functions), locality_results(logging), numsteps);

        // initialize stencil_values using the stencil (functional) components
        for (int i = 0; i < num_rows; ++i) 
            init_stencils(locality_results(stencils[i]), locality_results(functions), i, numvalues, par);

        // ask stencil instances for their output gids
        std::vector<std::vector<std::vector<naming::id_type> > > outputs(num_rows);
        for (int i = 0; i < num_rows; ++i) 
            get_output_ports(locality_results(stencils[i]), outputs[i]);

        // connect output gids with corresponding stencil inputs
        connect_input_ports(&*stencils.begin(), outputs,numvalues,par);

        // for loop over second row ; call start for each
        for (int i = 1; i < num_rows; ++i) 
            start_row(locality_results(stencils[i]));

        // do actual work
        execute(locality_results(stencils[0]), initial_data, result_data);

        // free all allocated components (we can do that synchronously)
        if (!logging.empty())
            factory.free_components_sync(logging);

        for (int i = 0; i < num_rows; ++i) 
            factory.free_components_sync(stencils[i]);
        factory.free_components_sync(functions);
#endif
        return result_data;

    }

    void unigrid_mesh::prep_ports(array3d &dst_port,array3d &dst_src,
                                  array3d &dst_step,array3d &dst_size,array3d &src_size,std::size_t num_rows,
                                  std::vector<std::size_t> &each_row,
                                  parameter const& par)
    {
      int i,j,k;
      
      // vcolumn is the destination column number
      // vstep is the destination step (or row) number
      // vsrc_column is the source column number
      // vsrc_step is the source step number
      // vport is the output port number; increases consecutively
      std::vector<int> vcolumn,vstep,vsrc_column,vsrc_step,vport;

      //using namespace boost::assign;

      int counter;
      int step,dst;
//      int found;

      std::size_t a,b,c;
      for (step=0;step<num_rows;step = step + 1) {
        for (i=0;i<each_row[step];i++) {
          counter = 0;

          // get 3D coordinates from 'i' value
          // i.e. i = a + nx*(b+c*nx);
          int level = findlevel3D(step,i,a,b,c,par);

          // communicate 27
          int kstart = c-1;
          if ( kstart < 0 ) kstart = 0;
          int kend = c+2;
          if ( kend > par->nx[level] ) kend = par->nx[level];

          int jstart = b-1;
          if ( jstart < 0 ) jstart = 0;
          int jend = b+2;
          if ( jend > par->nx[level] ) jend = par->nx[level];

          int istart = a-1;
          if ( istart < 0 ) istart = 0;
          int iend = a+2;
          if ( iend > par->nx[level] ) iend = par->nx[level];

          bool prolongation;
          dst = step;
          if ( (step+3)%3 != 0 || par->allowedl == 0 ) {
            // anytime there is a difference of more than one level between src and dst rows,
            // you need to account for the prolongation/restriction rows going on inbetween them.
            // That is given by 2^{L-l-1}-1
            int intermediate = (int) pow(2.,par->allowedl-level) ;
            if ( par->allowedl-level > 1 ) {
              dst += intermediate + intermediate/2 - 1;
            } else {
              dst += intermediate;
            }
            prolongation = false;
          } else {
            dst += 1; // this is a prolongation/restriction step
            prolongation = true;
          }
          if ( dst >= num_rows ) dst = 0;

          if ( prolongation == false ) {
            for (int kk=kstart;kk<kend;kk++) {
            for (int jj=jstart;jj<jend;jj++) {
            for (int ii=istart;ii<iend;ii++) {
              j = ii + par->nx[level]*(jj+kk*par->nx[level]);

              if ( level != par->allowedl ) {
                j += par->rowsize[level+1];
              }
              vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(j);vport.push_back(counter);
              counter++;
            }}}
          } else {
            j = i;
            vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(j);vport.push_back(counter);
            counter++;

            if ( level != par->allowedl ) { 
              // prolongation {{{
              // send data to higher level boundary points
              // determine which, if any, finer mesh cube share a common border with this coarser mesh cube

              // here is the bounding box of the coarser mesh cube
              double_type dx = par->dx0/pow(2.0,level);
              double_type fdx = par->dx0/pow(2.0,level+1);
              double_type xmin = par->min[level] + a*par->granularity*dx;
              double_type xmax = par->min[level] + (a*par->granularity+par->granularity-1)*dx; 
              double_type ymin = par->min[level] + b*par->granularity*dx;  
              double_type ymax = par->min[level] + (b*par->granularity+par->granularity-1)*dx;
              double_type zmin = par->min[level] + c*par->granularity*dx;  
              double_type zmax = par->min[level] + (c*par->granularity+par->granularity-1)*dx;

              // buffer
              xmin -= fdx;
              xmax += fdx;
              ymin -= fdx;
              ymax += fdx;
              zmin -= fdx;
              zmax += fdx;

              // First, check if this coarse mesh bounding box intersects the ghostzone region
              if ( intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->min[level+1]+par->gw*fdx) ||
                   intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1],
                                par->max[level+1]-par->gw*fdx,par->max[level+1]) ||
                   intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->min[level+1]+par->gw*fdx,
                                par->min[level+1],par->max[level+1]) ||
                   intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                par->min[level+1],par->max[level+1],
                                par->max[level+1]-par->gw*fdx,par->max[level+1],
                                par->min[level+1],par->max[level+1]) ||
                   intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                par->min[level+1],par->min[level+1]+par->gw*fdx,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1]) ||
                   intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                par->max[level+1]-par->gw*fdx,par->max[level+1],
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1]) ) 
              { 
                // the coarse mesh intersects the ghostzone region. Now find the finer mesh which 
                // intersects with this coarse mesh and the ghostzone region.
                for (int kk=0;kk<par->nx[level+1];kk++) {
                for (int jj=0;jj<par->nx[level+1];jj++) {
                for (int ii=0;ii<par->nx[level+1];ii++) {
                  double_type xmin2 = par->min[level+1] +  ii*par->granularity*fdx;
                  double_type xmax2 = par->min[level+1] + (ii*par->granularity+par->granularity-1)*fdx; 
                  double_type ymin2 = par->min[level+1] +  jj*par->granularity*fdx;  
                  double_type ymax2 = par->min[level+1] + (jj*par->granularity+par->granularity-1)*fdx;
                  double_type zmin2 = par->min[level+1] +  kk*par->granularity*fdx;  
                  double_type zmax2 = par->min[level+1] + (kk*par->granularity+par->granularity-1)*fdx;
                
                  if ( intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                    xmin2,xmax2,ymin2,ymax2,zmin2,zmax2) &&
                 (  intersection(xmin2,xmax2,ymin2,ymax2,zmin2,zmax2,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->min[level+1]+par->gw*fdx) ||
                   intersection(xmin2,xmax2,ymin2,ymax2,zmin2,zmax2,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1],
                                par->max[level+1]-par->gw*fdx,par->max[level+1]) ||
                   intersection(xmin2,xmax2,ymin2,ymax2,zmin2,zmax2,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->min[level+1]+par->gw*fdx,
                                par->min[level+1],par->max[level+1]) ||
                   intersection(xmin2,xmax2,ymin2,ymax2,zmin2,zmax2,
                                par->min[level+1],par->max[level+1],
                                par->max[level+1]-par->gw*fdx,par->max[level+1],
                                par->min[level+1],par->max[level+1]) ||
                   intersection(xmin2,xmax2,ymin2,ymax2,zmin2,zmax2,
                                par->min[level+1],par->min[level+1]+par->gw*fdx,
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1]) ||
                   intersection(xmin2,xmax2,ymin2,ymax2,zmin2,zmax2,
                                par->max[level+1]-par->gw*fdx,par->max[level+1],
                                par->min[level+1],par->max[level+1],
                                par->min[level+1],par->max[level+1]) ) ) {

                    j = ii + par->nx[level+1]*(jj+kk*par->nx[level+1]);
                    if ( level+1 != par->allowedl ) {
                      j += par->rowsize[level+2];
                    }
                    vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(j);vport.push_back(counter);
                    counter++;
                  }

                } } }
               

              }
              // }}}
            }

            if ( level != par->level_row[step] ) { 
              // restriction {{{
              // send data to the level immediately less than this one (for restriction)
              double_type dx = par->dx0/pow(2.0,level);
              double_type cdx = par->dx0/pow(2.0,level-1);
              double_type xmin = par->min[level] + a*par->granularity*dx;
              double_type xmax = par->min[level] + (a*par->granularity+par->granularity-1)*dx; 
              double_type ymin = par->min[level] + b*par->granularity*dx;  
              double_type ymax = par->min[level] + (b*par->granularity+par->granularity-1)*dx;
              double_type zmin = par->min[level] + c*par->granularity*dx;  
              double_type zmax = par->min[level] + (c*par->granularity+par->granularity-1)*dx;

              // first, check if this px thread is in the restriction region
              if ( intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                par->min[level]+par->gw*dx,par->max[level]-par->gw*dx,
                                par->min[level]+par->gw*dx,par->max[level]-par->gw*dx,
                                par->min[level]+par->gw*dx,par->max[level]-par->gw*dx) ) {
                // this px thread is in the restriction region.  Find out to which coarse thread we should restrict
                for (int kk=0;kk<par->nx[level-1];kk++) {
                for (int jj=0;jj<par->nx[level-1];jj++) {
                for (int ii=0;ii<par->nx[level-1];ii++) {
                  double_type xmin2 = par->min[level-1] +  ii*par->granularity*cdx;
                  double_type xmax2 = par->min[level-1] + (ii*par->granularity+par->granularity-1)*cdx; 
                  double_type ymin2 = par->min[level-1] +  jj*par->granularity*cdx;  
                  double_type ymax2 = par->min[level-1] + (jj*par->granularity+par->granularity-1)*cdx;
                  double_type zmin2 = par->min[level-1] +  kk*par->granularity*cdx;  
                  double_type zmax2 = par->min[level-1] + (kk*par->granularity+par->granularity-1)*cdx;

                  if ( intersection(xmin,xmax,ymin,ymax,zmin,zmax,
                                    xmin2,xmax2,ymin2,ymax2,zmin2,zmax2) ) {
                    // there is overlap -- some points from the specified finer mesh
                    // need to be sent to this particular coarse mesh
                    j = ii + par->nx[level-1]*(jj+kk*par->nx[level-1]);
                    j += par->rowsize[level];
                    vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(j);vport.push_back(counter);
                    counter++;
                  }
                } } }
              }
            // }}}
            }

          }
        }
      }

      // Create a ragged 3D array
      for (j=0;j<vsrc_step.size();j++) {
        int column,step,src_column,src_step,port;
        src_column = vsrc_column[j]; src_step = vsrc_step[j];
        column = vcolumn[j]; step = vstep[j];
        port = vport[j];
        dst_port( step,column,dst_size(step,column,0) ) = port;
        dst_src(  step,column,dst_size(step,column,0) ) = src_column;
        dst_step( step,column,dst_size(step,column,0) ) = src_step;
        dst_size(step,column,0) += 1;
        src_size(src_step,src_column,0) += 1;
      }

      // sort the src step (or row) in descending order
      int t1,kk;
      int column;
      for (j=0;j<vsrc_step.size();j++) {
        step = vstep[j];
        column = vcolumn[j];

        for (kk=dst_size(step,column,0);kk>=0;kk--) {
          for (k=0;k<kk-1;k++) {
            if (dst_step( step,column,k) < dst_step( step,column,k+1) ) {
              // swap
              t1 = dst_step( step,column,k);
              dst_step( step,column,k) = dst_step( step,column,k+1);
              dst_step( step,column,k+1) = t1;
  
              // swap the src, port info too
              t1 = dst_src( step,column,k);
              dst_src( step,column,k) = dst_src( step,column,k+1);
              dst_src( step,column,k+1) = t1;
  
              t1 = dst_port( step,column,k);
              dst_port( step,column,k) = dst_port( step,column,k+1);
              dst_port( step,column,k+1) = t1;
            } else if ( dst_step( step,column,k) == dst_step( step,column,k+1) ) {
              //sort the src column in ascending order if the step is the same
              if (dst_src( step,column,k) > dst_src( step,column,k+1) ) {
                t1 = dst_src( step,column,k);
                dst_src( step,column,k) = dst_src( step,column,k+1);
                dst_src( step,column,k+1) = t1;

                // swap the src, port info too
                t1 = dst_port( step,column,k);
                dst_port( step,column,k) = dst_port( step,column,k+1);
                dst_port( step,column,k+1) = t1;
              }

            }
          }
        }
      }
    }

    // This routine finds the level of a specified point given its row (step) and column (point)
    bool unigrid_mesh::intersection(double_type xmin,double_type xmax, 
                                    double_type ymin,double_type ymax, 
                                    double_type zmin,double_type zmax, 
                                    double_type xmin2,double_type xmax2, 
                                    double_type ymin2,double_type ymax2, 
                                    double_type zmin2,double_type zmax2) 
    {
      double_type pa[3],ea[3];
      static double_type const half = 0.5;
      pa[0] = half*(xmax + xmin);
      pa[1] = half*(ymax + ymin);
      pa[2] = half*(zmax + zmin);

      ea[0] = xmax - pa[0]; 
      ea[1] = ymax - pa[1]; 
      ea[2] = zmax - pa[2]; 

      double_type pb[3],eb[3];
      pb[0] = half*(xmax2 + xmin2);
      pb[1] = half*(ymax2 + ymin2);
      pb[2] = half*(zmax2 + zmin2);

      eb[0] = xmax2 - pb[0]; 
      eb[1] = ymax2 - pb[1]; 
      eb[2] = zmax2 - pb[2]; 

      double_type T[3];
      T[0] = pb[0] - pa[0];
      T[1] = pb[1] - pa[1];
      T[2] = pb[2] - pa[2];

      if ( floatcmp_le(fabs(T[0]),ea[0] + eb[0]) &&
           floatcmp_le(fabs(T[1]),ea[1] + eb[1]) &&
           floatcmp_le(fabs(T[2]),ea[2] + eb[2]) ) {
        return true;
      } else {
        return false;
      }

    }

    // This routine finds the level of a specified point given its row (step) and column (point)
    std::size_t unigrid_mesh::findlevel3D(std::size_t step, std::size_t item, std::size_t &a, std::size_t &b, std::size_t &c, parameter const& par)
    {
      int ll = par->level_row[step];
      // discover what level to which this point belongs
      int level = -1;
      if ( ll == par->allowedl ) {
        level = ll;
        // get 3D coordinates from 'i' value
        // i.e. i = a + nx*(b+c*nx);
        int tmp_index = item/par->nx[ll];
        c = tmp_index/par->nx[ll];
        b = tmp_index%par->nx[ll];
        a = item - par->nx[ll]*(b+c*par->nx[ll]);
        BOOST_ASSERT(item == a + par->nx[ll]*(b+c*par->nx[ll]));
      } else {
        if ( item < par->rowsize[par->allowedl] ) {
          level = par->allowedl;
        } else {
          for (int j=par->allowedl-1;j>=ll;j--) {
            if ( item < par->rowsize[j] && item >= par->rowsize[j+1] ) { 
              level = j;
              break;
            }
          }
        }
 
        if ( level < par->allowedl ) {
          int tmp_index = (item - par->rowsize[level+1])/par->nx[level];
          c = tmp_index/par->nx[level];
          b = tmp_index%par->nx[level];
          a = (item-par->rowsize[level+1]) - par->nx[level]*(b+c*par->nx[level]);
          BOOST_ASSERT(item-par->rowsize[level+1] == a + par->nx[level]*(b+c*par->nx[level]));
        } else {
          int tmp_index = item/par->nx[level];
          c = tmp_index/par->nx[level];
          b = tmp_index%par->nx[level];
          a = item - par->nx[level]*(b+c*par->nx[level]);
        }
      }
      BOOST_ASSERT(level >= 0);
      return level;
    }

}}}}