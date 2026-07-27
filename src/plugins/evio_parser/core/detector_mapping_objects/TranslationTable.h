#pragma once

#include <map>

#include "DAQAddress.h"
#include "DetectorAddress.h"

class TranslationTable {
public:
    bool Insert(DAQAddress daq, DetectorAddress detector);

    const DetectorAddress* Lookup(const DAQAddress& daq) const;

private:
    std::map<DAQAddress, DetectorAddress> m_entries;
};
