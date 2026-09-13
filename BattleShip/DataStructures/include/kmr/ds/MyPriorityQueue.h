#pragma once

#include "kmr/ds/MyArrayList.h"
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace kmr::ds {

/**
 * @class MyPriorityQueue
 * @author Kevin Munoz-Rivera
 * @brief Minimum-priority queue implemented as a binary heap.
 */
template <typename T>
class MyPriorityQueue {
public:
    /** @brief Inserts an item in O(log n) time. */
    void insert(const T& item) { heap_.add(item); bubbleUp(heap_.size() - 1); }
    void insert(T&& item) { heap_.add(std::move(item)); bubbleUp(heap_.size() - 1); }

    /** @brief Removes and returns the minimum item. */
    T removeMin() {
        if (heap_.isEmpty()) throw std::underflow_error("removeMin on empty MyPriorityQueue");
        T result = heap_.get(0);
        if (heap_.size() == 1) { heap_.remove(0); return result; }
        heap_.set(0, heap_.remove(heap_.size() - 1));
        sinkDown(0);
        return result;
    }

    /** @brief Returns the minimum item without removing it. */
    const T& min() const {
        if (heap_.isEmpty()) throw std::underflow_error("min on empty MyPriorityQueue");
        return heap_.get(0);
    }

    [[nodiscard]] std::size_t size() const noexcept { return heap_.size(); }
    [[nodiscard]] bool isEmpty() const noexcept { return heap_.isEmpty(); }
    [[nodiscard]] std::string toString() const { return heap_.toString(); }

private:
    MyArrayList<T> heap_;

    static std::size_t parent(std::size_t index) { return (index - 1) / 2; }
    static std::size_t left(std::size_t index) { return 2 * index + 1; }
    static std::size_t right(std::size_t index) { return 2 * index + 2; }

    void bubbleUp(std::size_t index) {
        while (index > 0) {
            const std::size_t p = parent(index);
            if (!(heap_.get(index) < heap_.get(p))) break;
            std::swap(heap_.get(index), heap_.get(p));
            index = p;
        }
    }

    void sinkDown(std::size_t index) {
        while (true) {
            const std::size_t l = left(index);
            if (l >= heap_.size()) return;
            const std::size_t r = right(index);
            std::size_t smallest = l;
            if (r < heap_.size() && heap_.get(r) < heap_.get(l)) smallest = r;
            if (!(heap_.get(smallest) < heap_.get(index))) return;
            std::swap(heap_.get(index), heap_.get(smallest));
            index = smallest;
        }
    }
};

}  // namespace kmr::ds
