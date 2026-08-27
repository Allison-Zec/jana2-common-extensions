#include <ModuleParser_HAPTB.h>

/**
 * @brief Parse a the raw data of the HAPPEX timing board and extract DAC16
 * 
 * Scans the data block from the HAPPEX timing baord which includes the 
 * DAC16 value.
 * 
 * @param data_block The data block to parse
 * @param rocid ROC ID for this data block
 * @param physics_events Reference to physics events vector (will be updated)
 * @param trigger_data Trigger data for the EVIO block
 */
void ModuleParser_HAPTB::parse(std::shared_ptr<evio::BaseStructure> data_block,
                               uint32_t rocid,
                               std::vector<PhysicsEvent*>& physics_events,
                               TriggerData& trigger_data) {
  std::vector<uint32_t> data_words = data_block->getUIntData();
  std::map<uint64_t, std::shared_ptr<EventHits_HAPTB>> event_hits_map;
  PhysicsEvent *phys_event = new PhysicsEvent();
  
  if (data_words.size() < 4) {
    throw JException("ModuleParser_HAPTB::parse: Not enough words (%zu) for a regular HAPPEX timing board event", data_words.size());
  }
  
  dac16 = getBitsInRange(data_words[0], 16, 0);
  event_number = trigger_data.first_event_number;
  
  
  if(event_hits_map.find(event_number) == event_hits_map.end()){
    event_hits_map[event_number] = std::make_shared<EventHits_HAPTB>();
  }
  
  HAPTBHit *current_hit = new HAPTBHit();
  current_hit->rocid = rocid;
  current_hit->event_num = event_number;
  current_hit->dac16 = dac16;
  
  event_hits_map[event_number]->hits_HAPTB.push_back(current_hit);
  
  phys_event->SetEventNumber(event_number);
  phys_event->addHits(event_hits_map[event_number]);
  
  LOG_DEBUG(GetLogger()) << "ModuleParser_HAPTB::DEBUG - ROCID = " << rocid<< "; Get Event Number = " << 
  phys_event->GetEventNumber() << "; Full Event Number = " << trigger_data.last_event_number << "; DAC16 = " << current_hit->dac16 << LOG_END;
  
  physics_events.push_back(phys_event);
}
 
