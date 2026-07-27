#include "TranslationTable.h"

bool TranslationTable::Insert(DAQAddress daq, DetectorAddress detector) {
    return m_entries.emplace(daq, detector).second;
}

const DetectorAddress* TranslationTable::Lookup(const DAQAddress& daq) const {
    const auto entry = m_entries.find(daq);
    return entry == m_entries.end() ? nullptr : &entry->second;
}
