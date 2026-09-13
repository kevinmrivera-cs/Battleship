#pragma once

#include <algorithm>
#include <bit>
#include <array>
#include <cstdint>
#include <limits>
#include <vector>

namespace kmr::ds {

/**
 * @class RadixSort
 * @author Kevin Munoz-Rivera
 * @brief Stable LSD radix sort utility for signed 32-bit integers.
 */
class RadixSort {
public:
    /** @brief Sorts integers ascending without comparison sorting. */
    static void sort(std::vector<int>& values) {
        // Transform signed order into unsigned lexicographic order by flipping the sign bit.
        std::vector<std::uint32_t> transformed;
        transformed.reserve(values.size());
        for (int value : values) transformed.push_back(std::bit_cast<std::uint32_t>(value) ^ 0x80000000u);

        std::vector<std::uint32_t> buffer(transformed.size());
        for (unsigned shift = 0; shift < 32; shift += 8) {
            std::array<std::size_t, 256> counts{};
            for (auto value : transformed) ++counts[(value >> shift) & 0xFFu];
            std::array<std::size_t, 256> offsets{};
            for (std::size_t i = 1; i < offsets.size(); ++i) offsets[i] = offsets[i - 1] + counts[i - 1];
            for (auto value : transformed) buffer[offsets[(value >> shift) & 0xFFu]++] = value;
            transformed.swap(buffer);
        }
        for (std::size_t i = 0; i < values.size(); ++i) values[i] = std::bit_cast<int>(transformed[i] ^ 0x80000000u);
    }
};

}  // namespace kmr::ds
