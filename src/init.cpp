/*
 * This file is part of CELADRO-3D-CUDA, Copyright (C) 2024, Siavash Monfared
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

 
#include "header.hpp"
#include "model.hpp"

using namespace std;


void Model::Initialize()
{

  for(unsigned k = 0 ; k < nphases_init ; k++){
  nphases_index.push_back(k);
  cellLineage(/*cell_id=*/k,/*parent_id=*/-1,/*birth_time=*/0,/*death_time=*/-1,/*physicalprop=*/omega_cc,/*generation=*/0);
  }
  

  if (BC == 4){
  Size[0] = Size[0] + 4.*wall_thickness;
  Size[1] = Size[1] + 4.*wall_thickness;
  cout<<"box adjusted for BC == 4"<<endl;
  }
  
  N = Size[0]*Size[1]*Size[2];
  sqrt_time_step = sqrt(time_step);

  // rectifies margin in case it is bigger than domain
  // and compensate for the boundary layer
  patch_margin = {
    min(margin, Size[0]/2 - 1 + (Size[0]%2)),
    min(margin, Size[1]/2 - 1 + (Size[1]%2)),
    min(margin, Size[2]/2 - 1 + (Size[2]%2))
  };
  // total size including bdry layer
  patch_size = 2u*patch_margin + 1u;
  
  patch_N = patch_size[0]*patch_size[1]*patch_size[2];
  // initialize memory for global fields
  walls.resize(N, 0.);
  walls_dx.resize(N, 0.);
  walls_dy.resize(N, 0.);
  walls_dz.resize(N, 0.);
  walls_laplace.resize(N, 0.);
  sum_one.resize(N, 0.);
  sum_two.resize(N, 0.);
  field_press.resize(N, 0.);
  field_polx.resize(N, 0.);
  field_poly.resize(N, 0.);
  field_polz.resize(N, 0.);   
  field_velx.resize(N, 0.);
  field_vely.resize(N, 0.);
  field_velz.resize(N, 0.);
  field_sxx.resize(N,0.);
  field_sxy.resize(N,0.);
  field_sxz.resize(N,0.);
  field_syy.resize(N,0.);
  field_syz.resize(N,0.);
  field_szz.resize(N,0.);
  
  /*
  sumS00.resize(N, 0.);
  sumS01.resize(N, 0.);
  sumS02.resize(N, 0.);
  sumS12.resize(N, 0.);
  sumS11.resize(N, 0.);
  sumS22.resize(N, 0.);
  sumQ00.resize(N, 0.);
  sumQ01.resize(N, 0.);
  square.resize(N, 0.);
  thirdp.resize(N, 0.);
  fourthp.resize(N, 0.);
  cIds.resize(N,0.);
  */

  // allocate memory for individual cells
  SetCellNumber(nphases_init);
  // ---------------------------------------------------------------------------
  if(zetaQ!=0.) sign_zetaQ = zetaQ>0. ? 1 : -1;
  if(zetaS!=0.) sign_zetaS = zetaS>0. ? 1 : -1;

  // compute tables
  for(unsigned i=0; i<Size[0]; ++i)
    com_x_table.push_back({ cos(-Pi+2.*Pi*i/Size[0]), sin(-Pi+2.*Pi*i/Size[0]) });
  for(unsigned i=0; i<Size[1]; ++i)
    com_y_table.push_back({ cos(-Pi+2.*Pi*i/Size[1]), sin(-Pi+2.*Pi*i/Size[1]) });
  for(unsigned i=0; i<Size[2]; ++i)
    com_z_table.push_back({ cos(-Pi+2.*Pi*i/Size[2]), sin(-Pi+2.*Pi*i/Size[2]) }); 

  // ---------------------------------------------------------------------------

  // check parameters
  for(unsigned n=0; n<nphases_init; ++n)
    if(margin<R) throw error_msg("Margin is too small, make it bigger than R.");

  // check birth boundaries
  if(birth_bdries.size()==0)
    birth_bdries = {0, Size[0], 0, Size[1], 0, Size[2]};
  else if(birth_bdries.size()!=4)
    throw error_msg("Birth boundaries have wrong format, see help.");

  //if(wall_omega!=0)
  //  throw error_msg("Wall adhesion is not working for the moment.");

  // ---------------------------------------------------------------------------

}

void Model::SetCellNumber(unsigned new_nphases)
{
  nphases = new_nphases;

  // allocate memory for qties defined on the patches
  phi.resize(nphases, vector<double>(patch_N, 0.));
  phi_dx.resize(nphases, vector<double>(patch_N, 0.));
  phi_dy.resize(nphases, vector<double>(patch_N, 0.));
  phi_dz.resize(nphases, vector<double>(patch_N, 0.));
  phi_old.resize(nphases, vector<double>(patch_N, 0.));
  V.resize(nphases, vector<double>(patch_N, 0.));
  dphi.resize(nphases, vector<double>(patch_N, 0.));
  dphi_old.resize(nphases, vector<double>(patch_N, 0.));
  // allocate memory for cell properties
  vol.resize(nphases, 0.);
  patch_min.resize(nphases, {0, 0, 0});
  patch_max.resize(nphases, Size);
  com.resize(nphases, {0., 0., 0.});
  com_prev.resize(nphases, {0., 0., 0.});
  polarization.resize(nphases, {0., 0., 0.});
  vorticity.resize(nphases,{0.,0.,0.});
  velocity.resize(nphases, {0., 0., 0.});
  Fpressure.resize(nphases, {0., 0., 0.});
  Fshape.resize(nphases, {0., 0., 0.});
  Fnem.resize(nphases, {0., 0., 0.});
  Fpol.resize(nphases, {0., 0., 0.});
  com_x.resize(nphases, 0.);
  com_y.resize(nphases, 0.);
  com_z.resize(nphases, 0.);
  cSxx.resize(nphases,0.);
  cSxy.resize(nphases,0.);
  cSxz.resize(nphases,0.);
  cSyy.resize(nphases,0.);
  cSyz.resize(nphases,0.);
  cSzz.resize(nphases,0.);

  offset.resize(nphases, {0u, 0u, 0u});
  theta_pol.resize(nphases, 0.);
  theta_pol_old.resize(nphases, 0.);
  delta_theta_pol.resize(nphases, 0.);
  
  stored_gam.resize(nphases,0.);
  stored_omega_cc.resize(nphases,0.);
  stored_omega_cs.resize(nphases,0.);
  stored_alpha.resize(nphases,0.);
  stored_dpol.resize(nphases,0.);
  timer.resize(nphases,0.);

 	
	divisiontthresh.resize(nphases);
	stored_tmean.resize(nphases);

	for (unsigned int i = 0; i < nphases; ++i) {
	 divisiontthresh[i] = prolif_start;
	 stored_tmean[i] = prolif_start + random_exponential(1./prolif_freq_mean);// mean = 1/lambda
	}

}

void Model::InitializeNeighbors()
{
  neighbors.resize(N);
  neighbors_patch.resize(patch_N);

  // define the neighbours, accounting for the periodic boundaries
  for(unsigned k=0; k<N; ++k)
  {
    const unsigned x = GetXPosition(k);
    const unsigned y = GetYPosition(k);
    const unsigned z = GetZPosition(k);
    for(int dx=-1; dx<=1; ++dx)
      for(int dy=-1; dy<=1; ++dy){
        for(int dz=-1; dz<=1; ++dz){        
          neighbors[k][dx][dy][dz] = GetIndex({ (x+Size[0]+dx)%Size[0], (y+Size[1]+dy)%Size[1], (z+Size[2]+dz)%Size[2] });
  
        }
      }
  }

  // define the neighbours, accounting for the boundary layer
  const unsigned lx = patch_size[0];
  const unsigned ly = patch_size[1];
  const unsigned lz = patch_size[2];
  for(unsigned k=0; k<patch_N; ++k)
  {
    const unsigned x = (k/ly)%lx;
    const unsigned y = k%ly;
    const unsigned z = k/(ly*lz);
    
    for(int dx=-1; dx<=1; ++dx)
    {
      for(int dy=-1; dy<=1; ++dy)
      {
        for(int dz=-1; dz<=1; ++dz)
        {
        const unsigned u = (x+lx+dx)%lx;
        const unsigned v = (y+ly+dy)%ly;
        const unsigned w = (z+lz+dz)%lz;
        neighbors_patch[k][dx][dy][dz] = v + u*ly + w * lx * ly;
          
        }
      }
    }
  }
}
