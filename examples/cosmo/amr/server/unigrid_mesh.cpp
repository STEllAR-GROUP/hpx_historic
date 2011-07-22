//  Copyright (c) 2007-2010 Hartmut Kaiser
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
#include <boost/assign/std/vector.hpp>

#include "../dynamic_stencil_value.hpp"
#include "../functional_component.hpp"
#include "../../parameter.hpp"

#include "unigrid_mesh.hpp"

///////////////////////////////////////////////////////////////////////////////
typedef hpx::components::amr::server::unigrid_mesh unigrid_mesh_type;

///////////////////////////////////////////////////////////////////////////////
// Serialization support for the actions
HPX_REGISTER_ACTION_EX(unigrid_mesh_type::init_execute_action, 
    cosmo_unigrid_mesh_init_execute_action);
HPX_REGISTER_ACTION_EX(unigrid_mesh_type::execute_action, 
    cosmo_unigrid_mesh_execute_action);

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(
    hpx::components::simple_component<unigrid_mesh_type>, cosmo_unigrid_mesh);
HPX_DEFINE_GET_COMPONENT_TYPE(unigrid_mesh_type);

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace amr { namespace server 
{
    unigrid_mesh::unigrid_mesh()
    {}

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
        Array3D &dst_port,Array3D &dst_src,Array3D &dst_step,
        Array3D &dst_size,Array3D &src_size, Parameter const& par)
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
        Array3D &dst_size,Array3D &dst_step,Array3D &dst_src,Array3D &dst_port,
        Parameter const& par)
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
        Parameter const& par)
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
    std::vector<naming::id_type> unigrid_mesh::init_execute(
        components::component_type function_type, std::size_t numvalues, 
        std::size_t numsteps,
        components::component_type logging_type,
        Parameter const& par)
    {
        //hpx::util::high_resolution_timer t;
        std::vector<naming::id_type> result_data;

        components::component_type stencil_type = 
            components::get_component_type<components::amr::server::dynamic_stencil_value>();

        typedef components::distributing_factory::result_type result_type;

        // create a distributing factory locally
        components::distributing_factory factory;
        factory.create(applier::get_applier().get_runtime_support_gid());

        // create a couple of stencil (functional) components and twice the 
        // amount of stencil_value components
        result_type functions = factory.create_components(function_type, numvalues);

        int num_rows = (int) pow(2,par->allowedl);
        num_rows *= 2; // we take two timesteps in the mesh

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
        for (int i=0;i<num_rows;i++) {
          int level = -1;
          for (int j=par->allowedl;j>=0;j--) {
            int tmp = (int) pow(2,j); 
            if ( i%tmp == 0 ) {
              level = par->allowedl-j;
              break;
            }
          }
          each_row.push_back(par->rowsize[level]);
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
        std::size_t memsize = 6;
        Array3D dst_port(num_rows,each_row[0],memsize);
        Array3D dst_src(num_rows,each_row[0],memsize);
        Array3D dst_step(num_rows,each_row[0],memsize);
        Array3D dst_size(num_rows,each_row[0],1);
        Array3D src_size(num_rows,each_row[0],1);
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
        execute(locality_results(stencils[0]), initial_data, result_data);

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
        components::component_type logging_type, Parameter const& par)
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

        had_double_type tmp = 3*pow(2,par->allowedl);
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

    void unigrid_mesh::prep_ports(Array3D &dst_port,Array3D &dst_src,
                                  Array3D &dst_step,Array3D &dst_size,Array3D &src_size,std::size_t num_rows,
                                  std::vector<std::size_t> &each_row,
                                  Parameter const& par)
    {
      int i,j,k;
      int adj_j;
      
      // vcolumn is the destination column number
      // vstep is the destination step (or row) number
      // vsrc_column is the source column number
      // vsrc_step is the source step number
      // vport is the output port number; increases consecutively
      std::vector<int> vcolumn,vstep,vsrc_column,vsrc_step,vport;

      int counter;
      int step,dst,dst2;
      //int found;
      int ghostpoint,ghostpoint_ok;
      int cghostpoint,cghostpoint_LoR;
      int ghostpoint2;
      int ghostpoint_LoR;
      int ghostpoint2_LoR;

      for (step=0;step<num_rows;step = step + 1) {
        for (i=0;i<each_row[step];i++) {
          counter = 0;

          // discover the level to which this point belongs 
          int level = -1;
          int ll = par->level_row[step];
          if ( ll == 0 ) {
            for (j=0;j<par->level_begin.size();j++) {
              if ( i >= par->level_begin[j] && i < par->level_end[j] ) {
                level = par->level_index[j];
                break;
              }
            }
          } else if ( ll == par->allowedl ) {
            level = ll;
          } else {
            for (j=0;j<par->level_begin.size();j++) {
              if ( par->level_index[j] >= ll && i >= par->level_begin[j]-par->offset[ll] && i < par->level_end[j]-par->offset[ll] ) {
                level = par->level_index[j];
                break;
              }
            }
          }
          BOOST_ASSERT(level >= 0);

          //std::cout << " TEST preports row " << step << " column " << i << " level " << level << std::endl;

          // communicate three 
          if ( step%2 == 0 || par->allowedl == 0 ) {
            // every column in the row is at the same timestep at this point

            // Discover what the destination row of this px thread is:
            //  'step' is the source row; 'i' is the source column; 
            //  'j' is the destination column (unknown at this point);
            //  'dst' (what we are searching for here) is the destination row.
            dst = step;
            dst += (int) pow(2,par->allowedl-level);
            if ( dst >= num_rows ) dst = 0;

            //std::cout << " TEST for row " << step << " column " << i << " level " << level << " dst " << dst << std::endl;
 
            // Check if point 'i' is a ghostpoint
            ghostpoint = 0;
            ghostpoint_LoR = -1;
            ghostpoint2_LoR = -1;
            for (j=0;j<par->ghostpoint.size();j++) {
              if ( i + par->offset[par->level_row[step]] == par->ghostpoint[j] 
                    && i != 0 && i != each_row[step]-1 ) {
                ghostpoint = 1;
                ghostpoint_LoR = par->ghostpoint_LoR[j];
                break;
              } 
            }
            if (ghostpoint == 1) {
              dst2 = step;
              dst2 += (int) pow(2,par->allowedl-(level-1));
              if ( dst2 >= num_rows ) dst2 = 0;
            }
            ghostpoint2 = 0;
            for (j=0;j<par->ghostpoint.size();j++) {
              if ( i + par->offset[par->level_row[step]] == par->ghostpoint[j] ) {
                ghostpoint2 = 1;
                ghostpoint2_LoR = par->ghostpoint_LoR[j];
                break;
              } 
            }
            // Check if point 'i' is a coarse ghostpoint (cghostpoint)
            cghostpoint = 0;
            cghostpoint_LoR = -1;
            for (j=0;j<par->cghostpoint.size();j++) {
              if ( i + par->offset[par->level_row[step]] == par->cghostpoint[j] ) {
                cghostpoint = 1;
                cghostpoint_LoR = par->cghostpoint_LoR[j];
                break;
              } 
            }
            if (cghostpoint == 1) {
              dst2 = step;
              dst2 += (int) pow(2,par->allowedl-(level+1));
              if ( dst2 >= num_rows ) dst2 = 0;
            }

            // communicate three px threads:
            for (j=i-1;j<i+2;j++) {
              // the meaning of 'j' depends upon both the destination row and the src row.
              adj_j = j - (par->offset[par->level_row[dst]]-par->offset[par->level_row[step]]);

              ghostpoint_ok = 1;
              if ( ghostpoint2 == 1 ) {
                if ( ghostpoint2_LoR == 0 && j == i-1) {
                  ghostpoint_ok = 0;
                } else if ( ghostpoint2_LoR == 1 && j == i+1 ) {
                  ghostpoint_ok = 0;
                }
              }
               
              if ( adj_j >=0 && adj_j < each_row[dst] && ghostpoint_ok == 1 ) {
                if ( cghostpoint == 1 && cghostpoint_LoR == 0 && j==i+1 ) {
                  adj_j = j - (par->offset[par->level_row[dst2]]-par->offset[par->level_row[step]]);
                  vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst2);vcolumn.push_back(adj_j);vport.push_back(counter);
                } else if (cghostpoint == 1 && cghostpoint_LoR == 1 && j==i-1 ) {
                  adj_j = j - (par->offset[par->level_row[dst2]]-par->offset[par->level_row[step]]);
                  vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst2);vcolumn.push_back(adj_j);vport.push_back(counter);
                } else {
                  vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(adj_j);vport.push_back(counter);
                }
                counter++;
              } else if (level == 0 ) {
                adj_j = j - (par->offset[par->level_row[dst]]-par->offset[par->level_row[step]]);
                if ( adj_j == -1 ) {
                  vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(each_row[dst]-1);vport.push_back(counter);
                  counter++;
                }
                if ( adj_j == each_row[dst] ) {
                  vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(0);vport.push_back(counter);
                  counter++;
                }
              }

            }
            if ( ghostpoint == 1 ) {
              if ( ghostpoint_LoR == 0 ) j=i-1;
              else j = i+1;
              adj_j = j - (par->offset[par->level_row[dst2]]-par->offset[par->level_row[step]]);
              if ( adj_j >=0 && adj_j < each_row[dst2] ) {
                vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst2);vcolumn.push_back(adj_j);vport.push_back(counter);
                counter++;
              }
            }

            
          } else {
            // the destination is the next immediate row
            dst = step+1;
            if ( dst == num_rows ) dst = 0;

            //std::cout << " TEST for row " << step << " column " << i << " level " << level << " dst " << dst << std::endl;
            ghostpoint = 0;
            ghostpoint_LoR = -1;
            ghostpoint2_LoR = -1;
            for (j=0;j<par->ghostpoint.size();j++) {
              if ( i + par->offset[par->level_row[step]] == par->ghostpoint[j] ) {
                ghostpoint = 1;
                ghostpoint_LoR = par->ghostpoint_LoR[j];
                break;
              } 
            }

            // only communicate between points on the same level
            for (j=i-1;j<i+2;j++) {
              // the meaning of 'j' depends upon both the destination row and the src row.
              adj_j = j - (par->offset[par->level_row[dst]]-par->offset[par->level_row[step]]);

              ghostpoint_ok = 1;
              if ( ghostpoint == 1 ) {
                if ( ghostpoint_LoR == 0 && j == i-1) {
                  ghostpoint_ok = 0;
                } else if ( ghostpoint_LoR == 1 && j == i+1 ) {
                  ghostpoint_ok = 0;
                }
              }

              if ( adj_j >=0 && adj_j < each_row[dst] && ghostpoint_ok == 1 ) {
                vsrc_step.push_back(step);vsrc_column.push_back(i);vstep.push_back(dst);vcolumn.push_back(adj_j);vport.push_back(counter);
                counter++;
              }
                   
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

      // Reorder for periodic boundary conditions
      int tt,tt2;
      int ss = (int) pow(2,par->allowedl);
      for (int row=0;row<num_rows;row=row+ss) {
        tt = dst_src(row,0,0); 
        tt2 = dst_src(row,0,1); 
        dst_src(row,0,0) = dst_src(row,0,2); 
        dst_src(row,0,1) = tt;
        dst_src(row,0,2) = tt2;

        tt = dst_port(row,0,0); 
        tt2 = dst_port(row,0,1); 
        dst_port(row,0,0) = dst_port(row,0,2); 
        dst_port(row,0,1) = tt;
        dst_port(row,0,2) = tt2;

        tt = dst_src(row,each_row[row]-1,1); 
        tt2 = dst_src(row,each_row[row]-1,2); 
        dst_src(row,each_row[row]-1,2) = dst_src(row,each_row[row]-1,0); 
        dst_src(row,each_row[row]-1,0) = tt;
        dst_src(row,each_row[row]-1,1) = tt2;

        tt = dst_port(row,each_row[row]-1,1); 
        tt2 = dst_port(row,each_row[row]-1,2); 
        dst_port(row,each_row[row]-1,2) = dst_port(row,each_row[row]-1,0); 
        dst_port(row,each_row[row]-1,0) = tt;
        dst_port(row,each_row[row]-1,1) = tt2;
      }

    }

    // this function returns the level corresponding to a particular row and column
    int unigrid_mesh::level_func(std::size_t row, std::size_t column, Parameter const& par)
    {
      int level = -1;
      for (int j=par->allowedl;j>=0;j--) {
        int tmp = (int) pow(2,j); 
        if ( row%tmp == 0 ) {
          level = par->allowedl-j;
          break;
        }
      }
      BOOST_ASSERT(level>=0);
     
      if ( level == par->allowedl ) return level;
      return -1;
    }

}}}}
