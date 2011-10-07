//  Copyright (c) 2007-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <hpx/hpx.hpp>

#if defined(RNPL_FOUND)
#include <sdf.h>
#endif

#include "logging.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace amr { namespace server
{
    logging::mutex_type logging::mtx_("logging");

    inline std::string convert(double d)
    {
      return boost::lexical_cast<std::string>(d);
    }


    ///////////////////////////////////////////////////////////////////////////
    // Implement actual functionality of this stencil
    // Compute the result value for the current time step
    void logging::logentry(stencil_data const& val, int row, int logcode, parameter const& par)
    {
        mutex_type::scoped_lock l(mtx_);

        if ( par->output_stdout == 1 ) {
          if (fmod(val.timestep_,par->output) < 1.e-6) {
            count++;  
            if ( count == 1000 ) {
                count = 0;
                std::cout << " PX Timestep: " <<  val.timestep_ 
                          << " PX Time: " << val.timestep_  
                          << " PX Level: " << val.level_  << std::endl;
            } 
#if 0
           // for (i=0;i<val.granularity*val.granularity*val.granularity;i++) {
              int i = 0;
              std::cout << " AMR Level: " << val.level_ 
                        << " Timestep: " <<  val.timestep_ 
                        << " Time: " << val.timestep_*par->dx0*par->lambda  
                        << " row: " << row 
                        << " index: " << val.index_ 
                        << " Value: " << val.value_[i].phi[0][0] 
                        << " x: " << val.x_[i] << " y: " << val.y_[i] << " z: " << val.z_[i] 
                        << std::endl << std::flush ;
          //  }
#endif
          }
        }

        // output to file "output.dat"
        if ( logcode == 0 ) {
#if 0
          FILE *fdata;
          fdata = fopen("equator.dat","a");
          double_type xx,yy,zz;
          int i,j,k;
          for (k=0;k<par->granularity;k++) {
          for (j=0;j<par->granularity;j++) {
          for (i=0;i<par->granularity;i++) {
            xx = val.x_[0]+i*par->dx0/pow(2.0,(int)val.level_);
            yy = val.y_[0]+j*par->dx0/pow(2.0,(int)val.level_);
            zz = val.z_[0]+k*par->dx0/pow(2.0,(int)val.level_);
            if ( zz <= 0.0 + 1.e-4 && zz >= 0.0 - 1.e-4 ) {
              fprintf(fdata,"%g %g %g %g\n",xx,yy,zz,val.timestep_*par->dx0*par->lambda);
            }
          } } }
          fclose(fdata);
#endif

#if defined(RNPL_FOUND)
#if 0
          int i;
          double datatime(0.0);
          std::vector<double> x,y,z,phi,d1phi,d2phi,d3phi,d4phi;
          if (fmod(val.timestep_,par->output) < 1.e-6 && val.level_ >= par->output_level) {
            for (i=0;i<par->granularity;i++) {
              x.push_back(val.x_[0]+i*par->dx0/pow(2.0,(int)val.level_));
            }
            for (i=0;i<par->granularity;i++) {
              x.push_back(val.y_[0]+i*par->dx0/pow(2.0,(int)val.level_));
            }
            for (i=0;i<par->granularity;i++) {
              x.push_back(val.z_[0]+i*par->dx0/pow(2.0,(int)val.level_));
            }

            for (i=0;i<par->granularity*par->granularity*par->granularity;i++) {
              phi.push_back(val.value_[i].phi[0][0]);
              d1phi.push_back(val.value_[i].phi[0][1]);
              d2phi.push_back(val.value_[i].phi[0][2]);
              d3phi.push_back(val.value_[i].phi[0][3]);
              d4phi.push_back(val.value_[i].phi[0][4]);
              datatime = val.timestep_*par->dx0*par->lambda;

//              std::string x_str = convert(val.x_[i]);
//              std::string chi_str = convert(val.value_[i].phi[0][0]);
//              std::string Phi_str = convert(val.value_[i].phi[0][1]);
//              std::string Pi_str = convert(val.value_[i].phi[0][2]);
//              std::string time_str = convert(val.timestep_*par->dx0*par->lambda);
//
//              FILE* fdata = fopen("chi.dat","a");
//              fprintf(fdata,"%d %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),chi_str.c_str());
//              fclose(fdata);
//
//              fdata = fopen("Phi.dat","a");
//              fprintf(fdata,"%d %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),Phi_str.c_str());
//              fclose(fdata);
//
//              fdata = fopen("Pi.dat","a");
//              fprintf(fdata,"%d %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),Pi_str.c_str());
//              fclose(fdata);
            }
            int shape[3];
            char cnames[80] = { "x|y|z" };
            char phi_name[] = { "0phi" };
            char phi1_name[] = { "0d1phi" };
            char phi2_name[] = { "0d2phi" };
            char phi3_name[] = { "0d3phi" };
            char phi4_name[] = { "0d4phi" };
            shape[0] = par->granularity;
            shape[1] = par->granularity;
            shape[2] = par->granularity;
            gft_out_full(phi_name,datatime,shape,cnames,3,&*x.begin(),&*phi.begin());
            gft_out_full(phi1_name,datatime,shape,cnames,3,&*x.begin(),&*d1phi.begin());
            gft_out_full(phi2_name,datatime,shape,cnames,3,&*x.begin(),&*d2phi.begin());
            gft_out_full(phi3_name,datatime,shape,cnames,3,&*x.begin(),&*d3phi.begin());
            gft_out_full(phi4_name,datatime,shape,cnames,3,&*x.begin(),&*d4phi.begin());
          }
#endif
#endif
        }
    }

}}}}

