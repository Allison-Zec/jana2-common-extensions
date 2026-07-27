#pragma once

#include <cstdint>

struct DetectorAddress {
    std::uint32_t plane;
    std::uint32_t bar;
    std::uint32_t signal;

    bool operator==(const DetectorAddress&) const = default;
};
