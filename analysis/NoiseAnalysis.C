#include "NoiseAnalysis.h"


WaveformStats AnalyzeWaveform(std::vector<uint32_t> *waveform){
  WaveformStats stats;
  stats.max_val = 0.0;
  stats.min_val = 1E16;
  stats.avg = 0.0;
  stats.rms = 0.0;
  for(Int_t i = 0; i < 300; i++){
    stats.avg += waveform->at(i);
    if(waveform->at(i) > stats.max_val){
      stats.max_val = waveform->at(i);
    }
    if(waveform->at(i) < stats.min_val){
      stats.min_val = waveform->at(i);
    }
  }
  stats.avg /= 300;
  
  for(Int_t i = 0; i < 300; i++){
    stats.rms += pow(waveform->at(i) - stats.avg, 2);
  }
  
  stats.rms = sqrt(stats.rms / 300);
  
  return stats;
}


void NoiseAnalysis(Int_t run_num, Int_t max_event=-1){
  TFile *infile = TFile::Open(Form("../Rootfiles/Run%i.root", run_num), "READ");
  TTree *waveform_tree = (TTree *)infile->Get("waveform_tree");
  
  std::vector<std::vector<Int_t>> all_cuts = ReadMaps(run_num);
  
  std::vector<WaveformStats> stats_vec;
  std::vector<uint32_t> *slot = nullptr;
  std::vector<uint32_t> *chan = nullptr;
  std::vector<uint32_t> *waveform_vec = nullptr;
  uint32_t dac16_in;
  
  waveform_tree->SetBranchAddress("waveform", &waveform_vec);
  waveform_tree->SetBranchAddress("slot", &slot);
  waveform_tree->SetBranchAddress("chan", &chan);
  waveform_tree->SetBranchAddress("dac16", &dac16_in);
  
  Int_t first_sync_bit = FindFirstSyncBit(run_num);
  Int_t max_entries = (max_event > 0) ? max_event : (Int_t)waveform_tree->GetEntries();
  std::vector<std::vector<Int_t>> loop_limits = GenerateLoopLimits(run_num, max_entries);
  
  for(Int_t i = 0; i < loop_limits.size(); i++){
    for(Int_t j = loop_limits[i][0] + 2; j < loop_limits[i][1]; j += 4){
      waveform_tree->GetEntry(j);
      if(waveform_vec->size() > 0){
        WaveformStats stats = AnalyzeWaveform(waveform_vec);
        stats.event_num = j;
        stats.dac16 = dac16_in;
        stats_vec.push_back(stats);
      }
    }
  }
  
  TH1F *hAvg = new TH1F("hAvg", Form("Run %i: Average Sample", run_num), 100, 380.0, 404.0);
  TH1F *hMax = new TH1F("hMax", Form("Run %i: Maximum Sample", run_num), 40, 390.0, 430.0);
  TH1F *hMin = new TH1F("hMin", Form("Run %i: Minimum Sample", run_num), 40, 350.0, 390.0);
  TH1F *hDiff = new TH1F("hDiff", Form("Run %i: Maximum - Minimum", run_num), 60, 12.0, 72.0);
  TH1F *hRMS = new TH1F("hRMS", Form("Run %i: RMS of Waveform", run_num), 100, 0.0, 25.0);
  
  for(Int_t i = 0; i < stats_vec.size(); i++){
    hAvg->Fill(stats_vec[i].avg);
    hMax->Fill(stats_vec[i].max_val);
    hMin->Fill(stats_vec[i].min_val);
    hDiff->Fill(stats_vec[i].max_val - stats_vec[i].min_val);
    hRMS->Fill(stats_vec[i].rms);
  }
  
  TCanvas *c1 = new TCanvas("c1", "Average Canvas", 1200, 800);
  c1->Divide(2, 2);
  c1->cd(1);
  hAvg->Draw();
  c1->cd(2);
  hMax->Draw();
  c1->cd(3);
  hMin->Draw();
  c1->cd(4);
  hDiff->Draw();
  
  TCanvas *c2 = new TCanvas("c2", "RMS Canvas", 1200, 800);
  hRMS->Draw();
}
