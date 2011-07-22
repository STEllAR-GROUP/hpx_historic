//  Copyright (c) 2007-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <hpx/hpx.hpp>

#if defined(SDF_FOUND)
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

#if MPFR_FOUND != 0
    inline std::string convert(mpfr::mpreal const & d)
    {
      return d.to_string();
    }
#endif


    ///////////////////////////////////////////////////////////////////////////
    // Implement actual functionality of this stencil
    // Compute the result value for the current time step
    void logging::logentry(stencil_data const& val, int row, int logcode, Parameter const& par)
    {
        mutex_type::scoped_lock l(mtx_);
        int i;

        if ( par->output_stdout == 1 ) {
          if (fmod(val.timestep_,par->output) < 1.e-6) {
            //for (i=0;i<val.granularity;i++) {
            for (i=0;i<1;i++) {
              std::cout << " AMR Level: " << val.level_ 
                        << " Timestep: " <<  val.timestep_ 
                        << " Time: " << val.timestep_*par->dx0*par->cfl  
                        << " row: " << row 
                        << " index: " << val.index_ 
                        << " Value: " << val.value_[i].phi[0][0] 
                        << " x-coordinate: " << val.x_[i] 
                        << std::endl << std::flush ;
            }
          }
        }

        // output to file "output.dat"
        FILE *fdata;
        std::vector<double> x,phi,Pi,chi;
        double datatime(0.0);
        if ( logcode == 0 ) {
          if (fmod(val.timestep_,par->output) < 1.e-6 && val.level_ >= par->output_level) {
            for (i=0;i<val.granularity;i++) {
              x.push_back(val.x_[i]);
              phi.push_back(val.value_[i].phi[0][0]);
              Pi.push_back(val.value_[i].phi[0][1]);
              chi.push_back(val.value_[i].phi[0][2]);
              datatime = val.timestep_*par->dx0*par->cfl;

              std::string x_str = convert(val.x_[i]);
              std::string phi_str = convert(val.value_[i].phi[0][0]);
              std::string Pi_str = convert(val.value_[i].phi[0][1]);
              std::string chi_str = convert(val.value_[i].phi[0][2]);
              std::string a_str = convert(val.value_[i].phi[0][3]);
              std::string f_str = convert(val.value_[i].phi[0][4]);
              std::string g_str = convert(val.value_[i].phi[0][5]);
              std::string b_str = convert(val.value_[i].phi[0][6]);
              std::string q_str = convert(val.value_[i].phi[0][7]);
              std::string r_str = convert(val.value_[i].phi[0][8]);
              std::string time_str = convert(val.timestep_*par->dx0*par->cfl);

              // FIXME: c stdio is bad

              fdata = fopen("phi.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),phi_str.c_str());
              fclose(fdata);

              fdata = fopen("Pi.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),Pi_str.c_str());
              fclose(fdata);

              fdata = fopen("chi.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),chi_str.c_str());
              fclose(fdata);

              fdata = fopen("a.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),a_str.c_str());
              fclose(fdata);

              fdata = fopen("f.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),f_str.c_str());
              fclose(fdata);

              fdata = fopen("g.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),g_str.c_str());
              fclose(fdata);

              fdata = fopen("b.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),b_str.c_str());
              fclose(fdata);

              fdata = fopen("q.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),q_str.c_str());
              fclose(fdata);

              fdata = fopen("r.dat","a");
              fprintf(fdata,"%ld %s %s %s\n",val.level_,time_str.c_str(),x_str.c_str(),r_str.c_str());
              fclose(fdata);
            }
#if defined(SDF_FOUND)
            int shape[3];
            char cnames[80] = { "r" };
            char phi_name[] = { "phi" };
            char pi_name[] = { "Pi" };
            char chi_name[] = { "chi" };
            shape[0] = x.size(); 
            gft_out_full(phi_name,datatime,shape,cnames,1,&*x.begin(),&*phi.begin());
            gft_out_full(pi_name,datatime,shape,cnames,1,&*x.begin(),&*Pi.begin());
            gft_out_full(chi_name,datatime,shape,cnames,1,&*x.begin(),&*chi.begin());
#endif
          }
        }

    }

}}}}

