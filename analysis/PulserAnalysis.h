#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <vector>
#include <iostream>


struct WaveformCalc {
  Float_t integral, pedestal, maxValue, minValue;
  Int_t max_bin, min_bin;
};

struct PulserResult {
  WaveformCalc waveform_calc;
  uint32_t dac16;
  Int_t cycle_step;
};


Float_t p1, p2, p3, p4;
Float_t p1ped, p2ped, p3ped, p4ped;
Float_t p1max, p2max, p3max, p4max, p1min, p2min, p3min, p4min;
Int_t p1maxBin, p2maxBin, p3maxBin, p4maxBin, p1minBin, p2minBin, p3minBin, p4minBin;
Int_t cycleNum;
Int_t dac16;
