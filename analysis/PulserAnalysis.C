#include "PulserAnalysis.h"


WaveformCalc calcWaveformIntegral(std::vector<uint32_t> *waveform){
  WaveformCalc waveform_results;

  Int_t samples = 300;
  Int_t pedestal_samp = 40;
  Float_t first_pedestal = 0.0;
  Float_t last_pedestal = 0.0;
  waveform_results.max_value = 0.0;
  waveform_results.min_value = 1E16;
  for(Int_t i = 0; i < pedestal_samp; i++){
    first_pedestal += waveform->at(i);
    last_pedestal += waveform->at(samples - pedestal_samp + i);
    
    if(waveform->at(i) > waveform_results.max_value){
      waveform_results.max_value = waveform->at(i);
      waveform_results.max_bin = i;
    }
    if(waveform->at(i) < waveform_results.min_value){
      waveform_results.min_value = waveform->at(i);
      waveform_results.min_bin = i;
    }
  }
  first_pedestal /= pedestal_samp;
  last_pedestal /= pedestal_samp;
  waveform_results.pedestal = (first_pedestal + last_pedestal) / 2.0;
  
  waveform_results.integral = 0.0;
  for(Int_t i = pedestal_samp; i < samples - pedestal_samp; i++){
    waveform_results.integral += waveform->at(i) - waveform_results.pedestal;
    if(waveform->at(i) > waveform_results.max_value){
      waveform_results.max_value = waveform->at(i);
      waveform_results.max_bin = i;
    }
    if(waveform->at(i) < waveform_results.min_value){
      waveform_results.min_value = waveform->at(i);
      waveform_results.min_bin = i;
    }
  }
  
  return waveform_results;
}

WaveformCalc emptyWaveCalc(){
  WaveformCalc empty;
  empty.integral = 0.0;
  empty.pedestal = 0.0;
  empty.max_value = 0.0;
  empty.min_value = 0.0;
  empty.max_bin = -1;
  empty.min_bin = -1;
  
  return empty;
}


void InitPulserTree(TTree *pulser){
  pulser->Branch("cycleNum", &cycleNum, "cycleNum/I");
  pulser->Branch("p1", &p1, "p1/F");
  pulser->Branch("p2", &p2, "p2/F");
  pulser->Branch("p3", &p3, "p3/F");
  pulser->Branch("p4", &p4, "p4/F");
  pulser->Branch("p1ped", &p1ped, "p1ped/F");
  pulser->Branch("p2ped", &p2ped, "p2ped/F");
  pulser->Branch("p3ped", &p3ped, "p3ped/F");
  pulser->Branch("p4ped", &p4ped, "p4ped/F");
  pulser->Branch("p1max", &p1max, "p1max/F");
  pulser->Branch("p2max", &p2max, "p2max/F");
  pulser->Branch("p3max", &p3max, "p3max/F");
  pulser->Branch("p4max", &p4max, "p4max/F");
  pulser->Branch("p1min", &p1min, "p1min/F");
  pulser->Branch("p2min", &p2min, "p2min/F");
  pulser->Branch("p3min", &p3min, "p3min/F");
  pulser->Branch("p4min", &p4min, "p4min/F");
  pulser->Branch("p1maxBin", &p1maxBin, "p1maxBin/I");
  pulser->Branch("p2maxBin", &p2maxBin, "p2maxBin/I");
  pulser->Branch("p3maxBin", &p3maxBin, "p3maxBin/I");
  pulser->Branch("p4maxBin", &p4maxBin, "p4maxBin/I");
  pulser->Branch("p1minBin", &p1minBin, "p1minBin/I");
  pulser->Branch("p2minBin", &p2minBin, "p2minBin/I");
  pulser->Branch("p3minBin", &p3minBin, "p3minBin/I");
  pulser->Branch("p4minBin", &p4minBin, "p4minBin/I");
  pulser->Branch("dac16", &dac16, "dac16/I");
}


std::vector<PulserResult> ParseAllWaveforms(Int_t runNum, TString infile_name, Int_t mode=0, Int_t max_entry=-1){
  TFile *infile = TFile::Open(infile_name.Data(), "READ");
  
  std::vector<uint32_t> *slot = nullptr;
  std::vector<uint32_t> *chan = nullptr;
  std::vector<uint32_t> *waveform_vec = nullptr;
  std::vector<PulserResult> result_list;
  Int_t cycle_step = -1;
  uint32_t dac16_in;
  std::vector<std::vector<Int_t>> all_cuts = ReadMaps(run_num);
  
  TTree *waveform = (TTree *)infile->Get("waveform_tree");
  TTree *pulse = (TTree *)infile->Get("pulse_tree");
  waveform->SetBranchAddress("waveform", &waveform_vec);
  waveform->SetBranchAddress("slot", &slot);
  waveform->SetBranchAddress("chan", &chan);
  waveform->SetBranchAddress("dac16", &dac16_in);
  
  
  Int_t stop_point = (Int_t)waveform->GetEntries();
  if(max_entry > 0){
    stop_point = max_entry;
  }
  
  Float_t pct = 0.05;
  Float_t pctThresh = pct * stop_point;
  for(Int_t i = 0; i < stop_point; i++){
    PulserResult pulser_result;
    waveform->GetEntry(i);
    if(chan->size() == 0 || waveform_vec->size() == 0){
      cycle_step = 0;
      pulser_result.waveform_calc = emptyWaveCalc();
      pulser_result.cycle_step = 0;
      pulser_result.dac16 = dac16_in;
      result_list.push_back(pulser_result);
      continue;
    }
    if(mode == 0){
      if(cycle_step == -1 && chan->at(0) == 0){
        continue;
      }
      if(chan->at(0) == 1){
        cycle_step = 0;
      }
    }
    if(mode == 1){
      if(cycle_step == -1 && chan->size() < 600){
        continue;
      }
      if(chan->size() == 600){
        cycle_step = 0;
      }
    }
    if(i > pctThresh){
      printf("Event %i, %.1f percent complete\n", i, 100.0 * pct);
      pct += 0.05;
      pctThresh = pct * stopPoint;
    }
    
    if(cycle_step > 3 || cycle_step < 0){
      printf("Cycle_Step has detected a bad cycle! Cycle_Step==%i\n", cycle_step);
    }
    
    pulser_result.waveform_calc = calcWaveformIntegral(waveform_vec);
    pulser_result.cycle_step = cycle_step;
    pulser_result.dac16 = dac16_in;
    result_list.push_back(pulser_result);
    
    cycle_step++;
  }
  
  infile->Close();
  
  return result_list;
}


void WriteOutputTree(std::vector<PulserResult> all_results, TString outfile_name, Int_t mode=0){
  TFile *outfile = TFile::Open(outfile_name.Data(), "RECREATE");
  TTree *pulser = new TTree("pulser", "pulser results tree");
  InitPulserTree(pulser);
  
  for(Int_t i = 0; i < all_results.size(); i++){
    PulserResult result = all_results[i];
    if((result.cycle_step == 0 && mode == 0) || (result.cycle_step == 2 && mode == 1)){
      p4       = result.waveform_calc.integral;
      p4ped    = result.waveform_calc.pedestal;
      p4max    = result.waveform_calc.max_value;
      p4min    = result.waveform_calc.min_value;
      p4maxBin = result.waveform_calc.max_bin;
      p4minBin = result.waveform_calc.min_bin;
    }
    else if((result.cycle_step == 1 && mode == 0) || (result.cycle_step == 3 && mode == 1)){
      p1       = result.waveform_calc.integral;
      p1ped    = result.waveform_calc.pedestal;
      p1max    = result.waveform_calc.max_value;
      p1min    = result.waveform_calc.min_value;
      p1maxBin = result.waveform_calc.max_bin;
      p1minBin = result.waveform_calc.min_bin;
      if(result.cycle_step == 3 && mode == 1){
        dac16 = result.dac16;
        pulser->Fill();
        cycle_num++;
      }
    }
    else if((result.cycle_step == 2 && mode == 0) || (result.cycle_step == 0 && mode == 1)){
      p2       = result.waveform_calc.integral;
      p2ped    = result.waveform_calc.pedestal;
      p2max    = result.waveform_calc.max_value;
      p2min    = result.waveform_calc.min_value;
      p2maxBin = result.waveform_calc.max_bin;
      p2minBin = result.waveform_calc.min_bin;
    }
    else if((result.cycle_step == 3 && mode == 0) || (result.cycle_step == 1 && mode == 1)){
      p3       = result.waveform_calc.integral;
      p3ped    = result.waveform_calc.pedestal;
      p3max    = result.waveform_calc.max_value;
      p3min    = result.waveform_calc.min_value;
      p3maxBin = result.waveform_calc.max_bin;
      p3minBin = result.waveform_calc.min_bin;
      if(result.cycle_step == 3 && mode == 0){
        dac16 = result.dac16;
        pulser->Fill();
        cycle_num++;
      }
    }
    else{
      printf("Bad pulser cycle at event %i!\n", i);
    }
  }
  
  outfile->cd();
  pulser->Write();
  outfile->Close();
}


void PlotMultipleTrees(TString infileName, Int_t maxEntry=-1){
  std::vector<uint32_t> *chan = nullptr;
  std::vector<uint32_t> *waveformVec = nullptr;
  std::vector<uint32_t> *integralSum = nullptr;
  uint32_t pedestalSum;
  
  TFile *infile = TFile::Open(infileName.Data(), "READ");
  TTree *waveTree = (TTree *)infile->Get("waveform_tree");
  TTree *pulseTree = (TTree *)infile->Get("pulse_tree");
  waveTree->SetBranchAddress("waveform", &waveformVec);
  waveTree->SetBranchAddress("chan", &chan);
  pulseTree->SetBranchAddress("integral_sum", &integralSum);
  pulseTree->SetBranchAddress("pedestal_sum", &pedestalSum);
  
  TCanvas *c = new TCanvas("c", "Comparison Canvas", 1200, 800);
  TGraph *gWave = new TGraph();
  gWave->SetName("gWave"); gWave->SetTitle("Waveform Tree (black) Pulse Tree (red)");
  gWave->GetXaxis()->SetTitle("Entry"); gWave->GetYaxis()->SetTitle("Integral Sum of Pulse");
  gWave->GetYaxis()->SetLimits(0, 1.8E5); gWave->GetYaxis()->SetRangeUser(0, 1.8E5);
  TGraph *gPulse = new TGraph();
  gPulse->SetName("gPulse"); gPulse->SetTitle("Pulse Tree Calculation");
  gWave->GetXaxis()->SetTitle("Entry"); gWave->GetYaxis()->SetTitle("Integral Sum of Pulse");
  gPulse->SetMarkerColor(kRed);
  
  Int_t waveEntries = 0;
  for(Int_t i = 0; i < waveTree->GetEntries(); i++){
    waveTree->GetEntry(i);
    WaveformCalc waveCalc = calcWaveformIntegral(waveformVec);
    if(chan->at(0) == 0){
      gWave->SetPoint(waveEntries, waveEntries, waveCalc.integral);
      waveEntries++;
    }
  }
  
  for(Int_t i = 0; i < pulseTree->GetEntries(); i++){
    pulseTree->GetEntry(i);
    Float_t pedestalSub = integralSum->at(0) - pedestalSum * 180.0 / 4.0;
    gPulse->SetPoint(i, i, pedestalSub);
  }
  
  c->cd();
  gWave->Draw("ap");
  gPulse->Draw("p && same");
}


void PulserAnalysis(Int_t run_num, Int_t mode=0, Int_t max_entry=-1){
  TString rootfilePath = "/adaqfs/home/acompton/software/HallA-compton-jana2/Rootfiles";
  TString infile_name = Form("%s/Run%i.root", rootfilePath.Data(), run_num);
  // TString infileName = "../evio_processor.root";
  TString outfile_name = Form("%s/Run%i_pulserwise.root", rootfilePath.Data(), run_num);
  // TString outfileName = Form("Run%i_pulserwise.root", runNum);
  std::vector<PulserResult> all_results = ParseAllWaveforms(run_num, infile_name, mode, max_entry);
  WriteOutputTree(all_results, outfile_name, mode);
  // PlotMultipleTrees(infileName, maxEntry);
}
