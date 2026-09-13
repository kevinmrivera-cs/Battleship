#pragma once
#include <cstddef>

namespace kmr::ds {

/**
 * @class Queue
 * @author Kevin Munoz-Rivera
 * @brief Minimal queue interface shared by queue implementations.
 */
template <typename T>
class Queue {
public:
    virtual ~Queue() = default;
    virtual void enqueue(const T& item) = 0;
    virtual T dequeue() = 0;
    virtual const T& front() const = 0;
    [[nodiscard]] virtual std::size_t size() const noexcept = 0;
    [[nodiscard]] virtual bool isEmpty() const noexcept = 0;
};

}  // namespace kmr::ds
