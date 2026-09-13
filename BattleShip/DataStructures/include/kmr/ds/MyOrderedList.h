#pragma once

#include "kmr/ds/MyArrayList.h"
#include <cstddef>
#include <stdexcept>
#include <string>

namespace kmr::ds {

/**
 * @class MyOrderedList
 * @author Kevin Munoz-Rivera
 * @brief Array-backed list that maintains ascending order and supports binary search.
 */
template <typename T>
class MyOrderedList {
public:
    /** @brief Inserts item at its sorted position. */
    void add(const T& item) {
        std::size_t lo = 0;
        std::size_t hi = items_.size();
        while (lo < hi) {
            const std::size_t mid = lo + (hi - lo) / 2;
            ++comparisons;
            if (items_.get(mid) < item) lo = mid + 1;
            else hi = mid;
        }
        items_.insert(item, lo);
    }

    /** @brief Finds and returns a pointer to the stored item, not the search argument. */
    T* binarySearch(const T& item) {
        const auto index = findIndex(item);
        return index >= 0 ? &items_.get(static_cast<std::size_t>(index)) : nullptr;
    }

    const T* binarySearch(const T& item) const {
        const auto index = findIndex(item);
        return index >= 0 ? &items_.get(static_cast<std::size_t>(index)) : nullptr;
    }

    /** @brief Removes the first matching item and returns whether removal occurred. */
    bool remove(const T& item) {
        const auto index = findIndex(item);
        if (index < 0) return false;
        items_.remove(static_cast<std::size_t>(index));
        return true;
    }

    T& get(std::size_t index) { return items_.get(index); }
    const T& get(std::size_t index) const { return items_.get(index); }
    [[nodiscard]] std::size_t size() const noexcept { return items_.size(); }
    [[nodiscard]] bool isEmpty() const noexcept { return items_.isEmpty(); }
    [[nodiscard]] std::string toString() const { return items_.toString(); }

    mutable std::size_t comparisons{0};

private:
    MyArrayList<T> items_;

    long long findIndex(const T& item) const {
        std::size_t lo = 0;
        std::size_t hi = items_.size();
        while (lo < hi) {
            const std::size_t mid = lo + (hi - lo) / 2;
            ++comparisons;
            if (items_.get(mid) < item) lo = mid + 1;
            else if (item < items_.get(mid)) hi = mid;
            else return static_cast<long long>(mid);
        }
        return -1;
    }
};

}  // namespace kmr::ds
