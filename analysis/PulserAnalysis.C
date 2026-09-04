#include "PulserAnalysis.h"


WaveformCalc calcWaveformIntegral(std::vector<uint32_t> *waveform){
  WaveformCalc waveform_results;

  Int_t samples = 300;
  Int_t pedestalSamp = 40;
  Float_t firstPedestal = 0.0;
  Float_t lastPedestal = 0.0;
  waveform_results.maxValue = 0.0;
  waveform_results.minValue = 1E16;
  for(Int_t i = 0; i < pedestalSamp; i++){
    firstPedestal += waveform->at(i);
    lastPedestal += waveform->at(samples - pedestalSamp + i);
    
    if(waveform->at(i) > waveform_results.maxValue){
      waveform_results.maxValue = waveform->at(i);
      waveform_results.max_bin = i;
    }
    if(waveform->at(i) < waveform_results.minValue){
      waveform_results.minValue = waveform->at(i);
      waveform_results.min_bin = i;
    }
  }
  firstPedestal /= pedestalSamp;
  lastPedestal /= pedestalSamp;
  waveform_results.pedestal = (firstPedestal + lastPedestal) / 2.0;
  
  waveform_results.integral = 0.0;
  for(Int_t i = pedestalSamp; i < samples - pedestalSamp; i++){
    waveform_results.integral += waveform->at(i) - waveform_results.pedestal;
    if(waveform->at(i) > waveform_results.maxValue){
      waveform_results.maxValue = waveform->at(i);
      waveform_results.max_bin = i;
    }
    if(waveform->at(i) < waveform_results.minValue){
      waveform_results.minValue = waveform->at(i);
      waveform_results.min_bin = i;
    }
  }
  
  return waveform_results;
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


std::vector<PulserResult> ParseAllWaveforms(Int_t runNum, TString infileName, Int_t maxEntry = -1){
  TFile *infile = TFile::Open(infileName.Data(), "READ");
  
  std::vector<uint32_t> *slot = nullptr;
  std::vector<uint32_t> *chan = nullptr;
  std::vector<uint32_t> *waveform_vec = nullptr;
  std::vector<PulserResult> result_list;
  Int_t cycleStep = -1;
  uint32_t dac16_in;
  
  TTree *waveform = (TTree *)infile->Get("waveform_tree");
  TTree *pulse = (TTree *)infile->Get("pulse_tree");
  waveform->SetBranchAddress("waveform", &waveform_vec);
  waveform->SetBranchAddress("slot", &slot);
  waveform->SetBranchAddress("chan", &chan);
  waveform->SetBranchAddress("dac16", &dac16_in);
  
  
  Int_t stopPoint = (Int_t)waveform->GetEntries();
  if(maxEntry > 0){
    stopPoint = maxEntry;
  }
  
  Float_t pct = 0.05;
  Float_t pctThresh = pct * stopPoint;
  for(Int_t i = 0; i < stopPoint; i++){
    PulserResult pulser_result;
    waveform->GetEntry(i);
    
    if(cycleStep == -1 && chan->at(0) == 0){
      continue;
    }
    if(chan->at(0) == 1){
      cycleStep = 0;
    }
    if(i > pctThresh){
      printf("Event %i, %.1f percent complete\n", i, 100.0 * pct);
      pct += 0.05;
      pctThresh = pct * stopPoint;
    }
    
    if(cycleStep > 3 || cycleStep < 0){
      printf("CycleStep has detected a bad cycle! CycleStep==%i\n", cycleStep);
    }
    
    pulser_result.waveform_calc = calcWaveformIntegral(waveform_vec);
    pulser_result.cycle_step = cycleStep;
    pulser_result.dac16 = dac16_in;
    result_list.push_back(pulser_result);
    
    cycleStep++;
  }
  
  infile->Close();
  
  return result_list;
}


void WriteOutputTree(std::vector<PulserResult> allResults, TString outfileName){
  TFile *outfile = TFile::Open(outfileName.Data(), "RECREATE");
  TTree *pulser = new TTree("pulser", "pulser results tree");
  InitPulserTree(pulser);
  
  for(Int_t i = 0; i < allResults.size(); i++){
    PulserResult result = allResults[i];
    if(result.cycle_step == 0){
      p4       = result.waveform_calc.integral;
      p4ped    = result.waveform_calc.pedestal;
      p4max    = result.waveform_calc.maxValue;
      p4min    = result.waveform_calc.minValue;
      p4maxBin = result.waveform_calc.max_bin;
      p4minBin = result.waveform_calc.min_bin;
    }
    else if(result.cycle_step == 1){
      p1       = result.waveform_calc.integral;
      p1ped    = result.waveform_calc.pedestal;
      p1max    = result.waveform_calc.maxValue;
      p1min    = result.waveform_calc.minValue;
      p1maxBin = result.waveform_calc.max_bin;
      p1minBin = result.waveform_calc.min_bin;
    }
    else if(result.cycle_step == 2){
      p2       = result.waveform_calc.integral;
      p2ped    = result.waveform_calc.pedestal;
      p2max    = result.waveform_calc.maxValue;
      p2min    = result.waveform_calc.minValue;
      p2maxBin = result.waveform_calc.max_bin;
      p2minBin = result.waveform_calc.min_bin;
    }
    else{
      p3       = result.waveform_calc.integral;
      p3ped    = result.waveform_calc.pedestal;
      p3max    = result.waveform_calc.maxValue;
      p3min    = result.waveform_calc.minValue;
      p3maxBin = result.waveform_calc.max_bin;
      p3minBin = result.waveform_calc.min_bin;
      dac16    = result.dac16;
      pulser->Fill();
      cycleNum++;
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


void PulserAnalysis(Int_t runNum, Int_t maxEntry=-1){
  // TString rootfilePath = "/adaqfs/home/acompton/software/HallA-compton-jana2/Rootfiles";
  // TString infileName = Form("%s/Run%i.root", rootfilePath.Data(), runNum);
  TString infileName = "../evio_processor.root";
  // TString outfileName = Form("%s/Run%i_pulserwise.root", rootfilePath.Data(), runNum);
  TString outfileName = Form("Run%i_pulserwise.root", runNum);
  std::vector<PulserResult> allResults = ParseAllWaveforms(runNum, infileName, maxEntry);
  WriteOutputTree(allResults, outfileName);
  // PlotMultipleTrees(infileName, maxEntry);
}
