#include <cassert>

#include "TranslationTable.h"

int main() {
    TranslationTable table;

    assert(table.Insert({1, 8, 0}, {1, 1, 0}));
    assert(!table.Insert({1, 8, 0}, {9, 9, 9}));

    const auto* mapped = table.Lookup({1, 8, 0});
    assert(mapped != nullptr);
    assert(*mapped == DetectorAddress({1, 1, 0}));

    assert(table.Lookup({1, 8, 1}) == nullptr);
}
