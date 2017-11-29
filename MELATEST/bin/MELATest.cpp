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
  std::unique_ptr<Mela> mela(new Mela(13, 125));
  double jpx_1 = 10;
  double jpy_1 = 10;
  double jpz_1 = 50;
  double jE_1 = sqrt(jpx_1*jpx_1 + jpy_1*jpy_1 + jpz_1*jpz_1);
  double jpx_2 = 10;
  double jpy_2 = 10;
  double jpz_2 = 50;
  double jE_2 = sqrt(jpx_2*jpx_2 + jpy_2*jpy_2 + jpz_2*jpz_2);
  double Hpx = -20;
  double Hpy = -20;
  double Hpz = -100;
  double HE = sqrt(Hpx*Hpx + Hpy*Hpy + Hpz*Hpz + 125*125);

  
  SimpleParticleCollection_t daughters, associated, mothers={};
  
  if(jE_1>=0) associated.emplace_back(0, TLorentzVector(jpx_1, jpy_1, jpz_1, jE_1));
  if(jE_2>=0) associated.emplace_back(0, TLorentzVector(jpx_2, jpy_2, jpz_2, jE_2));
  daughters.emplace_back(25, TLorentzVector(Hpx, Hpy, Hpz, HE));

  mela->setInputEvent(&daughters, &associated, &mothers, false); //false tells MELA that this is RECO event
  
  mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::JJQCD);
  float result;
  mela->computeProdP(result, true); 
  mela->resetInputEvent();
  std::cout << result << std::endl;
  return 0;
}
