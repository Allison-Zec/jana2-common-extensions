#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TROOT.h"
#include <vector>
#include <iostream>


Bool_t initialized = kFALSE;
Int_t curEvent = 0;

