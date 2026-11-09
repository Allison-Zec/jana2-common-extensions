#include "utils.h"


struct WaveformStats{
  Float_t max_val, min_val, avg, rms;
  Int_t event_num;
  uint32_t dac16;
};
