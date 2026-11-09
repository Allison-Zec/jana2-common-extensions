#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <map>

void CheckWaveformTree(Int_t run_num, TTree *waveform_tree){
  std::vector<uint32_t> *slot = nullptr;
  std::vector<uint32_t> *chan = nullptr;
  std::vector<uint32_t> *waveform_vec = nullptr;
  uint32_t dac16_in;
  
  waveform_tree->SetBranchAddress("waveform", &waveform_vec);
  waveform_tree->SetBranchAddress("slot", &slot);
  waveform_tree->SetBranchAddress("chan", &chan);
  waveform_tree->SetBranchAddress("dac16", &dac16_in);
  
  Int_t last_empty_event = 0;
  Int_t last_600_event = 0;
  for(Int_t i = 0; i < 60000; i++){
    waveform_tree->GetEntry(i);
    if(i < 300){
       printf("EVENT %i\n", i);
       printf("  Waveform size: %zu\n", waveform_vec->size());
       printf("  Channel size: %zu\n", chan->size());
       printf("  Slot size: %zu\n", slot->size());
    }
    if(chan->size() == 600 && i - last_600_event != 4){
      printf("Event #%i, DAC16 = %" PRIu32 " - Difference from last 600 event: %i\n", i, dac16_in, i - last_600_event);
    }
    else if(chan->size() == 0 && i - last_empty_event != 4){
      printf("Event #%i, DAC16 = %" PRIu32 " - Difference from last empty event: %i\n", i, dac16_in, i - last_empty_event);
    }
    if(chan->size() == 600){
      last_600_event = i;
    }
    else if(chan->size() == 0){
      last_empty_event = i;
    }
  }
  
  std::map<uint32_t, Int_t> dac_counts;
  std::map<uint32_t, Int_t> empty_event_counts;
  for(Int_t i = 0; i < waveform_tree->GetEntries(); i++){
    waveform_tree->GetEntry(i);
    if(dac_counts.find(dac16_in) == dac_counts.end()){
      dac_counts[dac16_in] = 0;
    }
    
    dac_counts[dac16_in] += 1;
    
    if(waveform_vec->size() == 0){
      if(empty_event_counts.find(dac16_in) == empty_event_counts.end()){
        empty_event_counts[dac16_in] = 0;
      }
      
      empty_event_counts[dac16_in] += 1;
    }
  }
  
  Int_t num_dacs = 0;
  Int_t total_events = 0;
  for(auto count : dac_counts){
    num_dacs++;
    printf("RUN %i, WAVEFORM: #%i: DAC Value %" PRIu32 " - with %i entries\n", run_num, num_dacs, count.first, count.second);
    total_events += count.second;
  }
  
  printf("RUN %i, WAVEFORM: Total events after reconstruction: %i\n", run_num, total_events);
  
  num_dacs = 0;
  for(auto count : empty_event_counts){
    num_dacs++;
    printf("RUN %i, WAVEFORM: #%i: DAC Value %" PRIu32 " - with %i entries\n", run_num, num_dacs, count.first, count.second);
  }
}


void CheckPulseTree(Int_t run_num, TTree *pulse_tree){
  std::vector<uint32_t> *integral_sum = nullptr;
  uint32_t pedestal_sum, dac16;
  
  pulse_tree->SetBranchAddress("integral_sum", &integral_sum);
  pulse_tree->SetBranchAddress("pedestal_sum", &pedestal_sum);
  pulse_tree->SetBranchAddress("dac16", &dac16);
  
  std::map<uint32_t, Int_t> dac_counts;
  for(Int_t i = 0; i < pulse_tree->GetEntries(); i++){
    pulse_tree->GetEntry(i);
    if(dac_counts.find(dac16) == dac_counts.end()){
      dac_counts[dac16] = 0;
    }
    
    dac_counts[dac16] += 1;
  }
  
  Int_t num_dacs = 0;
  Int_t total_events = 0;
  for(auto count : dac_counts){
    num_dacs++;
    printf("RUN %i, PULSE: #%i: DAC Value %" PRIu32 " - with %i entries\n", run_num, num_dacs, count.first, count.second);
    total_events += count.second;
  }
  
  printf("RUN %i, PULSE: Total events after reconstruction: %i\n", run_num, total_events);
}


void DataCheckScript(Int_t run_num){
  TString infile_name = Form("../Rootfiles/Run%i.root", run_num);
  // TString infile_name = "../evio_processor.root";
  TFile *infile = TFile::Open(infile_name.Data());
  
  TTree *waveform_tree = (TTree *)infile->Get("waveform_tree");
  TTree *pulse_tree = (TTree *)infile->Get("pulse_tree");
  CheckWaveformTree(run_num, waveform_tree);
  CheckPulseTree(run_num, pulse_tree);
  printf("Waveform Tree Entries: %lli\n", waveform_tree->GetEntries());
  printf("Pulse Tree Entries: %lli\n", pulse_tree->GetEntries());
}
