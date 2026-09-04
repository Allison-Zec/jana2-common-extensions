#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TString.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include <numeric>
#include <vector>


std::pair<Float_t, Float_t> CalculateStats(std::vector<Float_t> pulses){
  std::pair<Float_t, Float_t> statistics;
  
  Float_t sum = std::accumulate(pulses.begin(), pulses.end(), 0.0);
  Float_t mean = sum * 1.0 / pulses.size();
  
  Float_t sq_sum = 0.0;
  for(Int_t i = 0; i < pulses.size(); i++){
    sq_sum += (pulses[i] - mean) * (pulses[i] - mean);
  }
  
  Float_t std_err = std::sqrt(sq_sum) / pulses.size();
  
  statistics.first = mean;
  statistics.second = std_err;
  return statistics;
}


void PlotPulser(Int_t run_num){
  TString infile_path = ".";
  TString infile_name = Form("Run%i_pulserwise.root", run_num);
  
  std::map<Int_t, std::vector<Float_t>> bin_map_p3_p1;
  std::map<Int_t, std::vector<Float_t>> bin_map_p1;
  std::vector<Int_t> dac_values;
     
  Float_t p1, p2, p3;
  Int_t dac16;
  Int_t cur_dac = 0;
  Bool_t first_pass = kTRUE;
  
  TFile *infile = TFile::Open(Form("%s/%s", infile_path.Data(), infile_name.Data()));
  TTree *pulser = (TTree *)infile->Get("pulser");
  
  pulser->SetBranchAddress("p1", &p1);
  pulser->SetBranchAddress("p2", &p2);
  pulser->SetBranchAddress("p3", &p3);
  pulser->SetBranchAddress("dac16", &dac16);
  
  for(Int_t i = 0; i < pulser->GetEntries(); i++){
    pulser->GetEntry(i);
    if((dac16 > cur_dac) && first_pass){
      cur_dac = dac16;
      dac_values.push_back(dac16);
    }
    else if(dac16 < cur_dac){
      first_pass = kFALSE;
    }
    
    if(bin_map_p3_p1.find(dac16) == bin_map_p3_p1.end()){
      std::vector<Float_t> p3_p1_list;
      bin_map_p3_p1[dac16] = p3_p1_list;
    }
    bin_map_p3_p1[dac16].push_back(p3 - p1);
    
    if(bin_map_p1.find(dac16) == bin_map_p1.end()){
      std::vector<Float_t> p1_list;
      bin_map_p1[dac16] = p1_list;
    }
    bin_map_p1[dac16].push_back(p1);
  }
  
  infile->Close();
  
  TGraphErrors *gPulser = new TGraphErrors();
  gPulser->SetName("gPulser");
  gPulser->SetTitle("Variable - Delta Plot");
  gPulser->GetXaxis()->SetTitle("Sum(Variable)");
  gPulser->GetYaxis()->SetTitle("Sum(Variable + Delta) - Sum(Variable)");
  
  Int_t nPoints = 0;
  for(Int_t i = 0; i < dac_values.size(); i++){
    printf("Entry #%i: DAC16 = %i with %zu p3 - p1 entries\n", i, dac_values[i], bin_map_p3_p1[dac_values[i]].size());
    
    std::pair<Float_t, Float_t> p3_p1_stats = CalculateStats(bin_map_p3_p1[dac_values[i]]);
    std::pair<Float_t, Float_t> p1_stats = CalculateStats(bin_map_p1[dac_values[i]]);
    
    gPulser->SetPoint(nPoints, p1_stats.first, p3_p1_stats.first);
    gPulser->SetPointError(nPoints, 0.0, p3_p1_stats.second);
    nPoints++;
  }
  
  TCanvas *c = new TCanvas("c1", "Pulser Canvas", 1200, 800);
  gPulser->Draw("ap");
}
