#include "StepWaveforms.h"


void StepWaveform(Int_t runNum, Int_t eventNum=-1){
  if(eventNum >= 0){
    curEvent = eventNum;
  }
  
  TString rootfilePath = "/adaqfs/home/acompton/software/HallA-compton-jana2/Rootfiles";
  TFile *infile = TFile::Open(Form("%s/Run%i.root", rootfilePath.Data(), runNum), "READ");
  // TFile *outfile = TFile::Open(Form("Run%i_PulserAnalysis.root", runNum), "RECREATE");
  
  std::vector<uint32_t> *slot = nullptr;
  std::vector<uint32_t> *chan = nullptr;
  std::vector<uint32_t> *waveform_vec = nullptr;
  std::vector<Int_t> p4_entries;
  
  TTree *waveform = (TTree *)infile->Get("waveform_tree");
  waveform->SetBranchAddress("waveform", &waveform_vec);
  waveform->SetBranchAddress("slot", &slot);
  waveform->SetBranchAddress("chan", &chan);
  
  waveform->GetEntry(curEvent);
  
  TCanvas *c = (TCanvas *)gROOT->FindObject("c");
  TH1F *hWaveChan0 = (TH1F *)gROOT->FindObject("hWaveChan0");
  TH1F *hWaveChan1 = (TH1F *)gROOT->FindObject("hWaveChan1");
  
  if(!c){
    c = new TCanvas("c", "Waveform Canvas", 1200, 800);
  }
  if(!hWaveChan0 || !hWaveChan1){
    hWaveChan0 = new TH1F("hWaveChan0", Form("Run %i, event %i, chan %" PRIu32 "", runNum, curEvent, (Int_t)chan->at(0)), 300, 0.0, 300.0);
    hWaveChan1 = new TH1F("hWaveChan1", Form("Run %i, event %i, chan %" PRIu32 "", runNum, curEvent, (Int_t)chan->at(0)), 300, 0.0, 300.0);
    initialized = kTRUE;
  }
  
  curEvent++;
  Int_t channel = (Int_t)chan->at(0);
  Int_t size = (Int_t)waveform_vec->size();
  
  if(channel == 1){
    for(Int_t i = 0; i < 300; i++){
      hWaveChan1->SetBinContent(i + 1, waveform_vec->at(i));
    }
    hWaveChan1->Draw();
  }
  else if(channel == 0){
    for(Int_t i = 0; i < 300; i++){
      hWaveChan0->SetBinContent(i + 1, waveform_vec->at(i));
    }
    hWaveChan0->Draw();
  }
  
}
