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
#include "derivatives.hpp"

using namespace std;

void Model::print_new_cell_props(){
  
  unsigned n;
  for(unsigned i=0; i<nphases_index.size(); ++i){
  n = nphases_index[i];
  cout<<"n :"<<n<<" i: "<<i<<" "<<divisiontthresh[i]<<" "<<timer[i]<<endl;
  }
}


void Model::BirthCellMemories(unsigned new_nphases){

  phi.resize(new_nphases, vector<double>(patch_N, 0.));
  phi_dx.resize(new_nphases, vector<double>(patch_N, 0.));
  phi_dy.resize(new_nphases, vector<double>(patch_N, 0.));
  phi_dz.resize(new_nphases, vector<double>(patch_N, 0.));
  phi_old.resize(new_nphases, vector<double>(patch_N, 0.));
  V.resize(new_nphases, vector<double>(patch_N, 0.));
  dphi.resize(new_nphases, vector<double>(patch_N, 0.));
  dphi_old.resize(new_nphases, vector<double>(patch_N, 0.));
  vol.resize(new_nphases, 0.);
  patch_min.resize(new_nphases, {0, 0, 0});
  patch_max.resize(new_nphases, Size);
  com.resize(new_nphases, {0., 0., 0.});
  com_prev.resize(new_nphases, {0., 0., 0.});
  offset.resize(new_nphases, {0u, 0u, 0u});
  cSxx.resize(new_nphases,0.);
  cSxy.resize(new_nphases,0.);
  cSxz.resize(new_nphases,0.);
  cSyy.resize(new_nphases,0.);
  cSyz.resize(new_nphases,0.);
  cSzz.resize(new_nphases,0.);
  theta_pol.resize(new_nphases, 0.);
  theta_pol_old.resize(new_nphases, 0.);
  delta_theta_pol.resize(new_nphases, 0.);
  polarization.resize(new_nphases, {0., 0., 0.});
  vorticity.resize(new_nphases,{0.,0.,0.});
  velocity.resize(new_nphases, {0., 0., 0.});
  Fpressure.resize(new_nphases, {0., 0., 0.});
  Fshape.resize(new_nphases, {0., 0., 0.});
  //Fnem.resize(new_nphases, {0., 0., 0.});
  Fpol.resize(new_nphases, {0., 0., 0.});
  com_x.resize(new_nphases, 0.);
  com_y.resize(new_nphases, 0.);
  com_z.resize(new_nphases, 0.);
  
  stored_gam.resize(new_nphases,0.);
  stored_omega_cc.resize(new_nphases,0.);
  stored_omega_cs.resize(new_nphases,0.);
  stored_alpha.resize(new_nphases,0.);
  stored_dpol.resize(new_nphases,0.);
  
  timer.resize(new_nphases,0.);
  divisiontthresh.resize(new_nphases,0.);  
  stored_tmean.resize(new_nphases,0.);
  /*  
  S00.resize(new_nphases, 0.);
  S01.resize(new_nphases, 0.);
  S02.resize(new_nphases, 0.);
  S12.resize(new_nphases, 0.);
  S11.resize(new_nphases, 0.);
  S22.resize(new_nphases, 0.);
    
  Q00.resize(new_nphases, 0.);
  Q01.resize(new_nphases, 0.);
  */

}

void Model::DivideCell(unsigned n, unsigned idx, double division_orientation, double cellProp){

  double px = com[n][0];
  double py = com[n][1];
  double pz = com[n][2];
  
  //stored_gam[idx] = stored_gam[n];
  //stored_gam[idx-1] = stored_gam[n];
  
  stored_gam[idx] = cellProp;
  stored_gam[idx-1] = cellProp;
  
  stored_omega_cc[idx] = stored_omega_cc[n];
  stored_omega_cc[idx-1] = stored_omega_cc[n];

  stored_omega_cs[idx] = stored_omega_cs[n];
  stored_omega_cs[idx-1] = stored_omega_cs[n];
  
  stored_alpha[idx] = stored_alpha[n];
  stored_alpha[idx-1] = stored_alpha[n];
  
  stored_dpol[idx] = stored_dpol[n];
  stored_dpol[idx-1] = stored_dpol[n];
  
  patch_min[idx] = patch_min[n];
  patch_min[idx-1] = patch_min[n];
  patch_max[idx] = patch_max[n];
  patch_max[idx-1] = patch_max[n];
  
  offset[idx] = offset[n];
  offset[idx-1] = offset[n];
  
  timer[idx] = 0.;
  timer[idx-1] = 0.;
  
  //divisiontthresh[idx] = random_normal_full(tmean,(1./4.)*tmean);//tmean
  //divisiontthresh[idx-1] = random_normal_full(tmean,(1./4.)*tmean);//tmean
  
  // divisiontthresh[idx] = random_double_uniform(prolif_start*1.,prolif_start*10.);
  // divisiontthresh[idx-1] = random_double_uniform(prolif_start*1.,prolif_start*10.);
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, stored_tmean.size() - 1);
  int random_index = dis(gen);
  stored_tmean[idx] = stored_tmean[random_index];
  random_index = dis(gen);
  stored_tmean[idx-1] = stored_tmean[random_index];
   
  divisiontthresh[idx] = divisiontthresh[n];
  divisiontthresh[idx-1] = divisiontthresh[n];
  
  double rndir = random_uniform();
  double px1 = px + (R)*cos(rndir);
  double py1 = py + (R)*sin(rndir);
  double px2 = px + (R)*cos(rndir+M_PI);
  double py2 = py + (R)*sin(rndir+M_PI);

  for(unsigned q=0; q<patch_N; ++q){
  
  const auto      k = GetIndexFromPatch(n, q);
  const unsigned xk = GetXPosition(k);
  const unsigned yk = GetYPosition(k);
  const unsigned zk = GetZPosition(k);
  
  double pa[3] = {px1,py1,pz};
  double pb[3] = {px2,py2,pz};
  double pr[3] = {xk,yk,zk};
  double epsilon = 25.;
  
  double a[3] = {pa[0]-pb[0],pa[1]-pb[1],pa[2]-pb[2]};
  double norm = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
  a[0] = a[0] / norm;
  a[1] = a[1] / norm;
  a[2] = a[2] / norm;
  
  double b[3] ={pr[0]-(pa[0]+pb[0])/2.,pr[1]-(pa[1]+pb[1])/2.,pr[2]-(pa[2]+pb[2])/2.};
  double g = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
  double chi = (0.5)*(1+tanh(g/epsilon));
  
  phi[idx][q] = phi[n][q]*chi;
  phi[idx-1][q] = phi[n][q]*(1.-chi);
  
  phi_old[idx][q] = phi[n][q]*chi;
  phi_old[idx-1][q] = phi[n][q]*(1.-chi);
  
  }
}


void Model::ComputeBirthCellCOM(unsigned n, unsigned nbirth){

  double comx, comy, comz;
  comx = comy = comz = 0.;
  int count = 0;
  for(unsigned q=0; q<patch_N; ++q){
  
  const auto      k = GetIndexFromPatch(n, q);
  const unsigned xk = GetXPosition(k);
  const unsigned yk = GetYPosition(k);
  const unsigned zk = GetZPosition(k);
  
  if(phi[n][q] > 0.){
  comx = comx + xk;
  comy = comy + yk;
  comz = comz + zk;
  count = count + 1;
  }
  
  }
  comx = comx / count;
  comy = comy / count;
  comz = comz / count;
  com[n] = {com[nbirth][0],comy,com[nbirth][2]};
}

void Model::BirthCellAtNode(unsigned n, unsigned q)
{
    
  const auto      k = GetIndexFromPatch(n, q);

  phi_old[n][q] = phi[n][q];
  com_x[n] += com_x_table[GetXPosition(k)]*phi[n][q];
  com_y[n] += com_y_table[GetYPosition(k)]*phi[n][q];
  com_z[n] += com_z_table[GetZPosition(k)]*phi[n][q];
}
 
void Model::KillCell(unsigned n, unsigned i){

	for(unsigned q=0; q<patch_N; ++q){
		const auto k = GetIndexFromPatch(i, q);
		field_press[k] = 0.;
		sum_one[k] = 0;
		sum_two[k] = 0;
		field_polx[k] = 0.;
		field_poly[k] = 0.;
		field_polz[k] = 0.;
		field_velx[k] = 0;
		field_vely[k] = 0;
		field_velz[k] = 0;
	}

	phi.erase(phi.begin() + i);
	phi_old.erase(phi_old.begin() + i);
	V.erase(V.begin()+i);
	
	phi_dx.erase(phi_dx.begin()+i);
	phi_dy.erase(phi_dy.begin()+i);
	phi_dz.erase(phi_dz.begin()+i);
	
	dphi.erase(dphi.begin()+i);
	dphi_old.erase(dphi_old.begin()+i);

	com.erase(com.begin()+i);
	polarization.erase(polarization.begin()+i);
	velocity.erase(velocity.begin()+i);
	patch_min.erase(patch_min.begin()+i);
	patch_max.erase(patch_max.begin()+i);
	offset.erase(offset.begin()+i);
	vol.erase(vol.begin()+i);
	Fpol.erase(Fpol.begin()+i);
	cSxx.erase(cSxx.begin()+i);
	cSxy.erase(cSxy.begin()+i);
	cSxz.erase(cSxz.begin()+i);
	cSyy.erase(cSyy.begin()+i);
	cSyz.erase(cSyz.begin()+i);
	cSzz.erase(cSzz.begin()+i);
	Fpressure.erase(Fpressure.begin()+i);
	vorticity.erase(vorticity.begin()+i);
	delta_theta_pol.erase(delta_theta_pol.begin()+i);
	theta_pol.erase(theta_pol.begin()+i);
	theta_pol_old.erase(theta_pol_old.begin()+i);
	com_x.erase(com_x.begin()+i);
	com_y.erase(com_y.begin()+i);
	com_z.erase(com_z.begin()+i);
	
	nphases_index.erase(nphases_index.begin()+i);
	
	stored_gam.erase(stored_gam.begin()+i);
	stored_omega_cc.erase(stored_omega_cc.begin()+i);
	stored_omega_cs.erase(stored_omega_cs.begin()+i);
	stored_alpha.erase(stored_alpha.begin()+i);
	stored_dpol.erase(stored_dpol.begin()+i);
	timer.erase(timer.begin()+i);
	divisiontthresh.erase(divisiontthresh.begin()+i);
	stored_tmean.erase(stored_tmean.begin()+i);
	

}
/*
void Model::KillCell(unsigned n,unsigned i){
  for(unsigned q=0; q<patch_N; ++q){
  const auto k = GetIndexFromPatch(n, q);
  
  phi[n][q] = 0.;
  phi_old[n][q] = 0.;
  V[n][q] = 0.;
  pressure[k] = 0.;

  sum_one[k] = 0;
  sum_two[k] = 0;

  //sumS00[k] = 0;
  //sumS01[k] = 0;
  //sumS02[k] = 0;
  //sumS12[k] = 0;
  //sumS11[k] = 0;
  //sumS22[k] = 0;
  
  //sumQ00[k] = 0;
  //sumQ01[k] = 0;
  
  P0[k] = 0.;
  P1[k] = 0.;
  P2[k] = 0.;
  
  U0[k] = 0;
  U1[k] = 0;
  U2[k] = 0;
  
  }
  nphases_index.erase(nphases_index.begin()+i);
}
*/


void Model::BirthCell(unsigned n)
{

  // init polarisation and nematic
  theta_pol[n] = noise*Pi*(1-2*random_real());
  polarization[n] = { Spol*cos(theta_pol[n]), Spol*sin(theta_pol[n]) };
  
  /*
  theta_nem[n] = noise*Pi*(1-2*random_real());
  Q00[n] = Snem*cos(2*theta_nem[n]);
  Q01[n] = Snem*sin(2*theta_nem[n]);
  */

}







double Model::UpdateOU(double tcurrent, double tmean, double tcorr, double sigma, unsigned dt){
double ddt = static_cast<double>(dt);
double dW = std::sqrt(ddt) * random_normal(1.);
return tcurrent - ((tcurrent-tmean) / tcorr) * ddt + sigma * dW;
}



void Model::initDivisionOU(unsigned n, unsigned i, double division_orientation, unsigned t, bool mutate){
	double relt = static_cast<double>(t/(nsubsteps*ninfo));
	BirthCellMemories(nphases_index_head + 3);
	
	int cellGen = cellHist[n].generation + 1;
	double cellProp = stored_gam[i];
	if (mutate){
	cellProp = stored_gam[i] + stored_gam[i] * mutation_strength;
	if (cellProp > max_prop_val) cellProp = max_prop_val;
	if (cellProp < min_prop_val) cellProp = min_prop_val;
	}

	nphases_index_head = nphases_index_head + 1;
	GlobalCellIndex = GlobalCellIndex + 1;
	cellLineage(/*cell_id=*/GlobalCellIndex,/*parent_id=*/n,/*birth_time=*/relt,/*death_time=*/-1,/*physicalprop=*/cellProp,/*generation=*/cellGen);
	nphases_index.push_back(nphases_index_head);
	
	nphases_index_head = nphases_index_head + 1;
	GlobalCellIndex = GlobalCellIndex + 1;
	cellLineage(/*cell_id=*/GlobalCellIndex,/*parent_id=*/n,/*birth_time=*/relt,/*death_time=*/-1,/*physicalprop=*/cellProp,/*generation=*/cellGen);
	nphases_index.push_back(nphases_index_head);
	
	DivideCell(i,nphases_index_head,division_orientation,cellProp);
	BirthCell(nphases_index_head);
	BirthCell(nphases_index_head-1);
	ComputeBirthCellCOM(nphases_index_head,i);
	ComputeBirthCellCOM(nphases_index_head-1,i);
	cellLineage(/*cell_id=*/n,/*parent_id=*/-1,/*birth_time=*/-1,/*death_time=*/relt,/*physicalprop=*/gam,/*generation=*/cellGen);
	KillCell(n,i);
}



void Model::initDivision(unsigned n, unsigned i, double division_orientation, unsigned t){
	double relt = static_cast<double>(t/(nsubsteps*ninfo));
	BirthCellMemories(nphases_index_head + 3);
	
	int cellGen = cellHist[n].generation + 1;
	double cellProp = stored_gam[i] + stored_gam[i] * mutation_strength;

	if (cellProp > max_prop_val) cellProp = max_prop_val;
	if (cellProp < min_prop_val) cellProp = min_prop_val;

	nphases_index_head = nphases_index_head + 1;
	GlobalCellIndex = GlobalCellIndex + 1;
	cellLineage(/*cell_id=*/GlobalCellIndex,/*parent_id=*/n,/*birth_time=*/relt,/*death_time=*/-1,/*physicalprop=*/cellProp,/*generation=*/cellGen);
	nphases_index.push_back(nphases_index_head);
	
	nphases_index_head = nphases_index_head + 1;
	GlobalCellIndex = GlobalCellIndex + 1;
	cellLineage(/*cell_id=*/GlobalCellIndex,/*parent_id=*/n,/*birth_time=*/relt,/*death_time=*/-1,/*physicalprop=*/cellProp,/*generation=*/cellGen);
	nphases_index.push_back(nphases_index_head);
	
	DivideCell(i,nphases_index_head,division_orientation,cellProp);
	BirthCell(nphases_index_head);
	BirthCell(nphases_index_head-1);
	ComputeBirthCellCOM(nphases_index_head,i);
	ComputeBirthCellCOM(nphases_index_head-1,i);
	cellLineage(/*cell_id=*/n,/*parent_id=*/-1,/*birth_time=*/-1,/*death_time=*/relt,/*physicalprop=*/gam,/*generation=*/cellGen);
	KillCell(n,i);
}


std::vector<double> Model::compute_eigen(double sxx, double sxy, double syy){

        double trace = sxx + syy;
        double delta = std::sqrt((sxx - syy) * (sxx - syy) + 4.0 * sxy * sxy);
        double eig1 = 0.5 * (trace + delta);
        double eig2 = 0.5 * (trace - delta);

        double eigvalmax, eigvalmin;
        if (eig1 >= eig2) {
            eigvalmax = eig1;
            eigvalmin = eig2;
        } else {
            eigvalmax = eig2;
            eigvalmin = eig1;
        }

        // Compute eigenvectors.
        // For a symmetric 2x2 matrix, one common formula for the eigenvector of an eigenvalue λ is:
        //   (λ - syy, sxy) provided sxy is nonzero.
        double epsilon = std::numeric_limits<double>::epsilon();
        double v1_x, v1_y, v2_x, v2_y;
        if (std::abs(sxy) > epsilon) {
            v1_x = eigvalmax - syy;
            v1_y = sxy;
            v2_x = eigvalmin - syy;
            v2_y = sxy;
        } else {
            // Diagonal matrix: choose standard unit vectors.
            if (sxx >= syy) {
                v1_x = 1.0; v1_y = 0.0;
                v2_x = 0.0; v2_y = 1.0;
            } else {
                v1_x = 0.0; v1_y = 1.0;
                v2_x = 1.0; v2_y = 0.0;
            }
        }
        // Normalize eigenvectors.
        double norm1 = std::sqrt(v1_x * v1_x + v1_y * v1_y);
        double norm2 = std::sqrt(v2_x * v2_x + v2_y * v2_y);
        if (norm1 > epsilon) {
            v1_x /= norm1;
            v1_y /= norm1;
        }
        if (norm2 > epsilon) {
            v2_x /= norm2;
            v2_y /= norm2;
        }

	return {eigvalmax, eigvalmin, v1_x, v1_y, v2_x, v2_y};
}


void Model::cellLineage(int cell_id, int parent_id, double birth_t, double death_t, double prop, int gen)
{
    // 1) Check if cell_id already exists
    auto it = cellHist.find(cell_id);
    if (it != cellHist.end()) {
        // -- EXISTING CELL --
        // Only update fields that make sense for your scenario.
        // For example, if death_t != -1.0, update the death_time:
        if (death_t != -1.0) {
            it->second.death_time = death_t;
        }

        // If you also want to overwrite parent, generation, etc.:
        // it->second.parent     = parent_id;  
        // it->second.birth_time = birth_t;     
        // it->second.generation = gen;
        // (Or use sentinel logic to skip fields.)
    }
    else {
        // -- NEW CELL --
        cellInfo info;
        info.birth_time = birth_t;
        info.death_time = death_t;      // -1.0 => alive
        info.parent     = parent_id;    // -1 => no parent
        info.generation = gen;
        info.physicalprop = prop;


        // Insert in the global map
        cellHist[cell_id] = info;
    }
}


void Model::stress_criterionOU(unsigned pcellIndex, bool &mutate, double &angle) {
    double stressSum = 0.0;
    unsigned count = 0;
    const unsigned numPhases = nphases_index.size();

    // Compute average stress over all cells except pcellIndex.
    for (unsigned i = 0; i < numPhases; ++i) {
        const unsigned idx = nphases_index[i];
        if (idx != pcellIndex) {
            stressSum += (cSxx[idx] + cSyy[idx] + cSzz[idx]) / 3.0;
            ++count;
        }
    }
    double avgStress = (count > 0) ? stressSum / count : 0.0;

    // Compute hydrostatic stress for cell pcellIndex.
    double cellStress = (cSxx[pcellIndex] + cSyy[pcellIndex] + cSzz[pcellIndex]) / 3.0;
    mutate = (cellStress > avgStress);

    // Compute eigenvalues/eigenvectors for the cell's stress tensor.
    const double sxx = cSxx[pcellIndex];
    const double sxy = cSxy[pcellIndex];
    const double syy = cSyy[pcellIndex];
    const auto eigenResults = compute_eigen(sxx, sxy, syy);

    // Compute an angle from the eigenvector components.
    angle = std::atan2(eigenResults[3], eigenResults[2]);
}


std::vector<double> Model::stress_criterion() {
        unsigned best_i = 0;
        unsigned best_n = 0;
        std::vector<double> best_eigdirmax = {0.0, 0.0};
        double best_ratio = -std::numeric_limits<double>::infinity();

        for (unsigned i = 0; i < nphases_index.size(); ++i) {
            unsigned n = nphases_index[i];
            double sxx = cSxx[i];
            double sxy = cSxy[i];
            double syy = cSyy[i];

            std::vector<double> eigen_results = compute_eigen(sxx, sxy, syy);
            double eigvalmax = eigen_results[0];
            double eigvalmin = eigen_results[1];
            // eigen_results[2] and eigen_results[3] form the eigenvector for eigvalmax
            // Compute the ratio; (assumes eigvalmin is nonzero)
            double ratio = eigvalmax / eigvalmin;

            if (ratio > best_ratio) {
                best_ratio = ratio;
                best_i = i;
                best_n = n;
                best_eigdirmax = {eigen_results[2], eigen_results[3]};
            }
        }
	 double angle = std::atan2(best_eigdirmax[1], best_eigdirmax[0]);
        // Return the result as a list of doubles:
        // best phase index, corresponding n, eigenvector for the maximum eigenvalue (x and y components)
        return {static_cast<double>(best_i), static_cast<double>(best_n),angle};
                // best_eigdirmax[0], best_eigdirmax[1]};
    }


void Model::proliferate(unsigned t){
	if(proliferate_bool and (t) > prolif_start and nphases_index.size()*1. < nphases_max){
	// print_new_cell_props();
	for (unsigned i = 0 ; i < nphases_index.size(); i++){
	unsigned n = nphases_index[i];
	timer[i] += 1;
	divisiontthresh[i] = UpdateOU(divisiontthresh[i],stored_tmean[i],tcorr,sigma,1);
	// print_new_cell_props();
	if (timer[i] >= divisiontthresh[i]){
	bool mutate = false;
	double angle = 0.;
	stress_criterionOU(i,mutate,angle);
	GetFromDevice();
	FreeDeviceMemoryCellBirth();
	cout<<"dividing cell "<<n<<" with index "<<i<<" mutation "<<mutate<<" "<<timer[i]<<" "<<divisiontthresh[i]<<endl;
	initDivisionOU(n,i,angle,t,mutate);
	print_new_cell_props();
	nphases = nphases_index.size();
	nphases_index_head = nphases - 1;
	AllocDeviceMemoryCellBirth();
	PutToDevice();
	cout<<"proliferation complete; current number at "<<nphases<<endl;
	}
	}
}
}



/*
void Model::proliferate(unsigned t){

	if(proliferate_bool and t > prolif_start and remainder(tau_divide*1.,prolif_freq)==0. and nphases_index.size()*1. < nphases_max){
		std::vector<double> pre_prolif_info = stress_criterion();
		unsigned i = static_cast<unsigned>(pre_prolif_info[0]);
		unsigned n = static_cast<unsigned>(pre_prolif_info[1]);
		GetFromDevice();
		FreeDeviceMemoryCellBirth();
		//int imax = nphases_index.size() - 1;
		//unsigned i = random_int_uniform(0,imax);
		//unsigned n = nphases_index[i];
		cout<<"dividing cell "<<n<<" with index "<<i<<endl;
		initDivision(n,i,pre_prolif_info[2],t);
		nphases = nphases_index.size();
		nphases_index_head = nphases - 1;
		AllocDeviceMemoryCellBirth();
		PutToDevice();
		cout<<"proliferation complete; current number at "<<nphases<<endl;
		tau_divide = 0;
	}
	tau_divide = tau_divide + 1;
}
*/


void Model::write_cellHist_binary(const std::string &filename,
                                  unsigned currentTime,
                                  const std::map<int, cellInfo> &hist)
{
    std::ofstream out(filename, std::ios::binary | std::ios::app);
    if (!out) {
        std::cerr << "Could not open file " << filename << " for binary write.\n";
        return;
    }

    // 1) Write currentTime as double (8 bytes)
    double timeVal = static_cast<double>(currentTime);
    out.write(reinterpret_cast<const char*>(&timeVal), sizeof(timeVal));

    // 2) Write number of cells as int (4 bytes)
    int nCells = static_cast<int>(hist.size());
    out.write(reinterpret_cast<const char*>(&nCells), sizeof(nCells));

    // 3) For each cell, write:
    //    cellID (int, 4 bytes)
    //    birth_time (double, 8 bytes)
    //    death_time (double, 8 bytes)
    //    parent (int, 4 bytes)
    //    physicalprop (double, 8 bytes)
    //    generation (int, 4 bytes)
    for (auto &kv : hist) {
        int cellID = kv.first;
        const cellInfo &ci = kv.second;

        out.write(reinterpret_cast<const char*>(&cellID), sizeof(cellID));
        out.write(reinterpret_cast<const char*>(&ci.birth_time),    sizeof(ci.birth_time));
        out.write(reinterpret_cast<const char*>(&ci.death_time),    sizeof(ci.death_time));
        out.write(reinterpret_cast<const char*>(&ci.parent),        sizeof(ci.parent));
        out.write(reinterpret_cast<const char*>(&ci.physicalprop),  sizeof(ci.physicalprop));
        out.write(reinterpret_cast<const char*>(&ci.generation),    sizeof(ci.generation));
    }

    out.close();
}














