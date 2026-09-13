#pragma once

#include "kmr/ds/MyArrayList.h"
#include <cstddef>
#include <stdexcept>

namespace kmr::ds {

/**
 * @class ArrayStack
 * @author Kevin Munoz-Rivera
 * @brief LIFO stack backed by MyArrayList.
 */
template <typename T>
class ArrayStack {
public:
    /** @brief Pushes an item onto the top. */
    void push(const T& item) { items_.add(item); }
    /** @brief Removes and returns the top item. */
    T pop() {
        if (items_.isEmpty()) throw std::underflow_error("pop on empty ArrayStack");
        return items_.remove(items_.size() - 1);
    }
    /** @brief Returns the top item without removing it. */
    const T& top() const {
        if (items_.isEmpty()) throw std::underflow_error("top on empty ArrayStack");
        return items_.get(items_.size() - 1);
    }
    [[nodiscard]] std::size_t size() const noexcept { return items_.size(); }
    [[nodiscard]] bool isEmpty() const noexcept { return items_.isEmpty(); }
private:
    MyArrayList<T> items_;
};

}  // namespace kmr::ds
