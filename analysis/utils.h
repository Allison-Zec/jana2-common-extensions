#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


Int_t FindFirstSyncBit(Int_t run_num){
  TFile *infile = TFile::Open(Form("../Rootfiles/Run%i.root", run_num), "READ");
  TTree *waveform_tree = (TTree *)infile->Get("waveform_tree");
  
  std::vector<uint32_t> *waveform_vec = nullptr;
  
  waveform_tree->SetBranchAddress("waveform", &waveform_vec);
  Int_t first_sync_bit = 0;
  for(Int_t i = 0; i < 4; i++){
    waveform_tree->GetEntry(i);
    if(waveform_vec->size() > 300){
      if(waveform_vec->at(300) > 1000){
        if(i < 4){
          first_sync_bit = i;
        }
      }
    }
  }
  
  infile->Close();
  return first_sync_bit;
}


std::vector<std::vector<Int_t>> ReadMaps(Int_t run_num){
  std::vector<std::vector<Int_t>> all_cuts;
  
  std::ifstream file(Form("maps/Run%i.map", run_num));
  if(!file.is_open()) {
    return all_cuts;
  }
  
  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string cell;
    std::vector<Int_t> row;

    while (std::getline(ss, cell, ',')) {
      Int_t event_num = std::stoi(cell);
      row.push_back(event_num);
    }
    all_cuts.push_back(row);
  }
  
  return all_cuts;
}


std::vector<std::vector<Int_t>> GenerateLoopLimits(Int_t run_num, Int_t max_entries){
  std::vector<std::vector<Int_t>> all_cuts = ReadMaps(run_num);
  std::vector<std::vector<Int_t>> loop_limits;
  Int_t first_sync_bit = FindFirstSyncBit(run_num);
  
  if(all_cuts.size() == 0){
    std::vector<Int_t> limits = {first_sync_bit, max_entries};
    loop_limits.push_back(limits);
  }
  else{
    Int_t prev_start_event = first_sync_bit;
    Bool_t closed = kFALSE;
    for(Int_t i = 0; i < all_cuts.size(); i++){
      if(all_cuts[i][0] < max_entries && all_cuts[i][1] <= max_entries){
        std::vector<Int_t> limits = {prev_start_event, all_cuts[i][0]};
        loop_limits.push_back(limits);
        prev_start_event = all_cuts[i][1];
      }
      else if(all_cuts[i][0] < max_entries && all_cuts[i][1]){
        std::vector<Int_t> limits = {prev_start_event, all_cuts[i][0]};
        loop_limits.push_back(limits);
        closed = kTRUE;
        break;
      }
      else if(all_cuts[i][0] >= max_entries && all_cuts[i][1] > max_entries){
        std::vector<Int_t> limits = {prev_start_event, max_entries};
        loop_limits.push_back(limits);
        closed = kTRUE;
        break;
      }
    }
    if(!closed){
      std::vector<Int_t> limits = {prev_start_event, max_entries};
      loop_limits.push_back(limits);
    }
  }
  
  return loop_limits;
}
