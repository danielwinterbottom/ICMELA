#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdint.h>

#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "ZZMatrixElement/MELA/interface/Mela.h"

int main(int argc, char* argv[]){
  
  double jpx_1 = 100;
  double jpy_1 = 10;
  double jpz_1 = 50;
  double jE_1 = sqrt(jpx_1*jpx_1 + jpy_1*jpy_1 + jpz_1*jpz_1);
  double jpx_2 = 0;
  double jpy_2 = 15;
  double jpz_2 = 45;
  double jE_2 = sqrt(jpx_2*jpx_2 + jpy_2*jpy_2 + jpz_2*jpz_2);
  double Hpx = -50;
  double Hpy = -25;
  double Hpz = -195;
  double HE = sqrt(Hpx*Hpx + Hpy*Hpy + Hpz*Hpz + 125*125);
  
  std::unique_ptr<Mela> mela(new Mela(13, 125));
 
  SimpleParticleCollection_t daughters, associated;
  
  if(jE_1>=0) associated.emplace_back(0, TLorentzVector(jpx_1, jpy_1, jpz_1, jE_1));
  if(jE_2>=0) associated.emplace_back(0, TLorentzVector(jpx_2, jpy_2, jpz_2, jE_2));
  daughters.emplace_back(25, TLorentzVector(Hpx, Hpy, Hpz, HE));
  
  mela->setInputEvent(&daughters, &associated, NULL, false); //false tells MELA that this is RECO event
  
  float cpodd_result=0, cpeven_result=0, mela_obs=0;
  
  mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::JJQCD);
  mela->computeProdP(cpodd_result, true); 
  mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::JJQCD);
  mela->computeProdP(cpeven_result, true);

  mela_obs = cpodd_result/(cpeven_result+cpodd_result);

  std::cout << cpodd_result << "    " << cpeven_result << "    " << mela_obs << std::endl;
  mela->resetInputEvent();
  return 0;
}
