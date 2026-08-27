#ifndef MODULEPARSER_HAPTB_H
#define MODULEPARSER_HAPTB_H

#include "ModuleParser.h"
#include "HAPTBHit.h"
#include "EventHits_HAPTB.h"

#include <JANA/JException.h>

/**
 * @class ModuleParser_HAPTB
 * @brief A module to parse the HAPPEX Timing Board data and get the DAC16 value
 */
class ModuleParser_HAPTB : public ModuleParser {
  public:
    void parse(std::shared_ptr<evio::BaseStructure> data_block,
              uint32_t rocid,
              std::vector<PhysicsEvent *>& physics_events,
              TriggerData& trigger_data) override;
         
  private:    
    uint32_t dac16;
    uint64_t event_number;
};
#endif // MODULEPARSER_HAPTB_H
