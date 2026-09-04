#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <vector>
#include <iostream>


void VerifySums(Int_t runNum){
  TString rootfilePath = "/adaqfs/home/acompton/software/HallA-compton-jana2/Rootfiles";
  TFile *infile = TFile::Open(Form("%s/Run%i.root", rootfilePath.Data(), runNum), "READ");  
  TTree *pulse_tree = (TTree *)infile->Get("pulse_tree");
  TTree *waveform_tree = (TTree *)infile->Get("waveform_tree");
  
  std::vector<uint32_t> *p_slot = nullptr;
  std::vector<uint32_t> *p_chan = nullptr;
  std::vector<uint32_t> *integral_sum = nullptr;
  std::vector<uint32_t> *w_slot = nullptr;
  std::vector<uint32_t> *w_chan = nullptr;
  std::vector<uint32_t> *waveform = nullptr;
  pulse_tree->SetBranchAddress("integral_sum", &integral_sum);
  pulse_tree->SetBranchAddress("slot", &p_slot);
  pulse_tree->SetBranchAddress("chan", &p_chan);
  waveform_tree->SetBranchAddress("slot", &w_slot);
  waveform_tree->SetBranchAddress("chan", &w_chan);
  waveform_tree->SetBranchAddress("waveform", &waveform);

  Int_t entryLimit = pulse_tree->GetEntries() > 2000 ? 2000 : pulse_tree->GetEntries();
  
  printf("Waveform entries: %i\n", (Int_t)waveform_tree->GetEntries());
  printf("Pulse entries: %i\n", (Int_t)pulse_tree->GetEntries());
  printf("Waveform entries (channel 0): %i\n", (Int_t)waveform_tree->GetEntries("chan==0"));
  printf("Pulse entries (channel 0): %i\n", (Int_t)pulse_tree->GetEntries("chan==0"));
  printf("Waveform entries (channel 1): %i\n", (Int_t)waveform_tree->GetEntries("chan==1"));
  printf("Pulse entries (channel 1): %i\n", (Int_t)pulse_tree->GetEntries("chan==1"));
  
  for(Int_t i = 0; i < entryLimit; i++){
    pulse_tree->GetEntry(i);
  }
}
