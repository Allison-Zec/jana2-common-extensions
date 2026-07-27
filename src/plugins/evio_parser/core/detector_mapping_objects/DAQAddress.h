#pragma once

#include <compare>
#include <cstdint>

struct DAQAddress {
    std::uint32_t rocid;
    std::uint32_t slot;
    std::uint32_t channel;

    auto operator<=>(const DAQAddress&) const = default;
};
