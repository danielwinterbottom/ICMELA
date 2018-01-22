#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdint.h>

#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "ZZMatrixElement/MELA/interface/Mela.h"

void CalculateProbabilitiesGGH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix);
void CalculateProbabilitiesVBF(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix);
void CalculateProbabilitiesWlepH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix);
void CalculateProbabilitiesWhadH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix);
void CalculateProbabilitiesZlepH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix);
void CalculateProbabilitiesZhadH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix);
void CalculateDiscriminators(float probCPEven, float probCPOdd, float probCPMix, float& discriminatorD0Minus, float& discriminatorDCP);
int main(int argc, char* argv[]){
    
  if (argc != 2 && argc != 3){
    std::cerr << "Need 1 or 2 args: <input> (<file_prefix>)" << std::endl;
    exit(1);
  }
  
  std::string file_prefix = "";
  if(argc==3) file_prefix = argv[2];
  std::string input_file = argv[1];
  std::string output_file = input_file;
  
  if (output_file.find("input.root") != input_file.npos) {
    std::size_t pos = output_file.find("input.root");
    output_file.replace(pos, std::string("input.root").length(), "output.root");
  } else {
    std::cerr << "The input file is not named correctly" << std::endl;
    return 1;
  }
  TFile *input = TFile::Open((file_prefix+input_file).c_str());
  if (!input) {
    std::cerr << "The input file could not be opened" << std::endl;
    return 1;
  }
  TTree *itree = dynamic_cast<TTree *>(input->Get("mela"));
  if (!itree) {
    std::cerr << "The input tree could not be found" << std::endl;
    return 1;
  }
  
  unsigned event = 0;
  unsigned lumi = 0;
  unsigned run = 0;
  std::vector<TLorentzVector> jets;
  TLorentzVector higgs;
  float D0, DCP;
  
  TH1::AddDirectory(kFALSE);
  
  itree->SetBranchAddress("event", &event);
  itree->SetBranchAddress("lumi", &lumi);
  itree->SetBranchAddress("run", &run);
  itree->SetBranchAddress("jets", &jets);
  itree->SetBranchAddress("higgs", &higgs);
  
  TFile *output = new TFile(output_file.c_str(),"RECREATE");
  TTree *otree = new TTree("mela","mela");
  otree->Branch("event", &event, "event/i");
  otree->Branch("lumi", &lumi, "lumi/i");
  otree->Branch("run", &run, "run/i");
  otree->Branch("D0", &D0);
  otree->Branch("DCP", &DCP);
  
  std::unique_ptr<Mela> mela = std::unique_ptr<Mela>(new Mela(13.0, 125.0, TVar::SILENT));
  
  for (unsigned i = 0; i < itree->GetEntries(); ++i) {
    itree->GetEntry(i);
    SimpleParticleCollection_t daughters, associated;
  
    for (unsigned i=0; i<jets.size(); ++i) associated.emplace_back(0,jets[i]);
    daughters.emplace_back(25, higgs);
    mela->setInputEvent(&daughters, &associated, nullptr, false); //false tells MELA that this is RECO event
    float probCPEven, probCPOdd, probCPMix;
    CalculateProbabilitiesGGH(mela, probCPEven, probCPOdd, probCPMix);
    CalculateDiscriminators(probCPEven, probCPOdd, probCPMix, D0, DCP);
    otree->Fill();
    std::cout << D0 << "    " << DCP << std::endl;
  }
  mela->resetInputEvent();
  
  output->Write();
  delete otree;
  output->Close();
  delete output;

  input->Close();
  delete input;

  return 0;
}

void CalculateProbabilitiesGGH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix)
{
  // CP even
  m_mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::JJQCD);
  m_mela->computeProdP(probCPEven, false);
  
  // CP odd
  m_mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::JJQCD);
  m_mela->computeProdP(probCPOdd, false);
  
  // CP mixing (maximum)
  m_mela->setProcess(TVar::SelfDefine_spin0, TVar::JHUGen, TVar::JJQCD);
  m_mela->selfDHggcoupl[0][gHIGGS_GG_2][0] = 1; // a1
  m_mela->selfDHggcoupl[0][gHIGGS_GG_4][0] = 1; // a3
  m_mela->computeProdP(probCPMix, false);
}

void CalculateProbabilitiesVBF(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix)
{
  // CP even
  m_mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::JJVBF);
  m_mela->computeProdP(probCPEven, false);
  
  // CP odd
  m_mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::JJVBF);
  m_mela->computeProdP(probCPOdd, false);
  
  // CP mixing (maximum)
  m_mela->setProcess(TVar::SelfDefine_spin0, TVar::JHUGen, TVar::JJVBF);
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_1][0] = 1; // a1
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_4][0] = 0.297979; // a3
  m_mela->computeProdP(probCPMix, false);
}

void CalculateProbabilitiesWlepH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix)
{
  // CP even
  m_mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::Lep_WH);
  m_mela->computeProdP(probCPEven, false);
  
  // CP odd
  m_mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::Lep_WH);
  m_mela->computeProdP(probCPOdd, false);
  
  // CP mixing (maximum)
  m_mela->setProcess(TVar::SelfDefine_spin0, TVar::JHUGen, TVar::Lep_WH);
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_1][0] = 1; // a1
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_4][0] = 0.1236136; // a3
  m_mela->computeProdP(probCPMix, false);
}

void CalculateProbabilitiesWhadH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix)
{
  // CP even
  m_mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::Had_WH);
  m_mela->computeProdP(probCPEven, false);
  
  // CP odd
  m_mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::Had_WH);
  m_mela->computeProdP(probCPOdd, false);
  
  // CP mixing (maximum)
  m_mela->setProcess(TVar::SelfDefine_spin0, TVar::JHUGen, TVar::Had_WH);
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_1][0] = 1; // a1
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_4][0] = 0.1236136; // a3
  m_mela->computeProdP(probCPMix, false);
}

void CalculateProbabilitiesZlepH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix)
{
  // CP even
  m_mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::Lep_ZH);
  m_mela->computeProdP(probCPEven, false);
  
  // CP odd
  m_mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::Lep_ZH);
  m_mela->computeProdP(probCPOdd, false);
  
  // CP mixing (maximum)
  m_mela->setProcess(TVar::SelfDefine_spin0, TVar::JHUGen, TVar::Lep_ZH);
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_1][0] = 1; // a1
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_4][0] = 0.144057; // a3
  m_mela->computeProdP(probCPMix, false);
}

void CalculateProbabilitiesZhadH(std::unique_ptr<Mela> const& m_mela, float& probCPEven, float& probCPOdd, float& probCPMix)
{
  // CP even
  m_mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::Had_ZH);
  m_mela->computeProdP(probCPEven, false);
  
  // CP odd
  m_mela->setProcess(TVar::H0minus, TVar::JHUGen, TVar::Had_ZH);
  m_mela->computeProdP(probCPOdd, false);
  
  // CP mixing (maximum)
  m_mela->setProcess(TVar::SelfDefine_spin0, TVar::JHUGen, TVar::Had_ZH);
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_1][0] = 1; // a1
  m_mela->selfDHzzcoupl[0][gHIGGS_VV_4][0] = 0.144057; // a3
  m_mela->computeProdP(probCPMix, false);
}

void CalculateDiscriminators(float probCPEven, float probCPOdd, float probCPMix,
                                           float& discriminatorD0Minus, float& discriminatorDCP)
{
  if ((probCPEven + probCPOdd) != 0.0)
  {
    discriminatorD0Minus = probCPEven / (probCPEven + probCPOdd);
    discriminatorDCP = (probCPMix - probCPEven - probCPOdd) / (probCPEven + probCPOdd);
  }
  else
  {
    discriminatorD0Minus = -9999;
    discriminatorDCP = -9999;
  }
}

