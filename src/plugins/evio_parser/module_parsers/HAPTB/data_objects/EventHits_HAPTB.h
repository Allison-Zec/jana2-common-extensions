#ifndef _EVENT_HITS_HAPTB_H_
#define _EVENT_HITS_HAPTB_H_

#include <JANA/JEvent.h>

#include "EventHits.h"
#include "HAPTBHit.h"


/**
 * @class EventHits_HAPTB
 * @brief A class containing a list of 
 */
class EventHits_HAPTB : public EventHits{
  public:
    std::vector<HAPTBHit *> hits_HAPTB;
    
    void insertIntoEvent(JEvent &event) override {
      for(auto &hit_HAPTB : hits_HAPTB){
        event.Insert(hit_HAPTB);
      }
    }
};

#endif // _EVENT_HITS_HAPTB_H_
