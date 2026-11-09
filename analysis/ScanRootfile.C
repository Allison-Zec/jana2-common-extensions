#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <fstream> 
#include <vector>


void ScanRootfile(Int_t run_num){
  TFile *infile = TFile::Open(Form("../Rootfiles/Run%i.root", run_num), "READ");
  TTree *waveform_tree = (TTree *)infile->Get("waveform_tree");
  
  std::vector<uint32_t> *waveform_vec = nullptr;
  waveform_tree->SetBranchAddress("waveform", &waveform_vec);
  
  Int_t first_sync_bit = 0;
  Int_t cur_sync_bit = -1;
  Int_t event_thresh = waveform_tree->GetEntries() / 100;
  Int_t cur_thresh = event_thresh;
  Int_t cur_pct = 1;
  
  printf("Scanning file for waveform misses. This may take some time...\n");
  
  for(Int_t i = 0; i < waveform_tree->GetEntries(); i++){
    waveform_tree->GetEntry(i);
    if(waveform_vec->size() > 300){
      if(waveform_vec->at(300) > 1000){
        if(i < 4){
          first_sync_bit = i;
        }
        if(i - cur_sync_bit != 4 && cur_sync_bit > 3){
          printf("Sync bit mismatch found! Current event %i with previous bit %i\n", i, cur_sync_bit);
        }
        cur_sync_bit = i;
        if(i > cur_thresh){
          printf("Scan is %i%% done!\n", cur_pct);
          cur_thresh += event_thresh;
          cur_pct++;
        }
      }
    }
  }
} 
