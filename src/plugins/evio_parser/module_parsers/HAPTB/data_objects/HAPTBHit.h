#ifndef HAPTBHIT_H
#define HAPTBHIT_H

#include <cstdint>

/**
 * @class HAPTBHit
 * @brief Class to register a hit from the HAPPEX Timing Board
 * 
 * Right now the only really important variable from the HAPTB 
 * is the DAC16 value which is stored as a uint32_t
 */
class HAPTBHit{
  public:
    uint32_t rocid
    uint32_t slot;
    uint32_t dac16;
  
    HAPTBHit() :
      rocid(0),
      slot(0),
      dac16(0) {}
};
#endif //HAPTBHIT_H
