#include "StepWaveforms.h"


void StepWaveform(Int_t runNum, Int_t eventNum=-1, Int_t force_channel=0){
  if(eventNum >= 0){
    curEvent = eventNum;
  }
  
  TString rootfilePath = "/adaqfs/home/acompton/software/HallA-compton-jana2/Rootfiles";
  TFile *infile = TFile::Open(Form("%s/Run%i.root", rootfilePath.Data(), runNum), "READ");
  
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
  if((!hWaveChan0 || !hWaveChan1) && chan->size() > 0){
    hWaveChan0 = new TH1F("hWaveChan0", Form("Run %i, event %i, chan %" PRIu32 ", Vector Size %zu", runNum, curEvent, (Int_t)chan->at(0), chan->size()), 300, 0.0, 300.0);
    hWaveChan1 = new TH1F("hWaveChan1", Form("Run %i, event %i, chan %" PRIu32 ", Vector Size %zu", runNum, curEvent, (Int_t)chan->at(0), chan->size()), 300, 0.0, 300.0);
    initialized = kTRUE;
  }
  
  curEvent++;
  if(chan->size() == 0 || waveform_vec->size() == 0){
    hWaveChan1 = new TH1F("hWaveChan1", Form("Run %i, event %i, NO WAVEFORM DATA", runNum, curEvent - 1), 300, 0.0, 300.0);
    for(Int_t i = 1; i < 301; i++){
      hWaveChan1->SetBinContent(i + 1, 0.0);
    }
    hWaveChan1->Draw();
  }
  else{
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
}
