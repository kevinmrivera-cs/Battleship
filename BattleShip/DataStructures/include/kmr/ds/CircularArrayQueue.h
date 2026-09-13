#pragma once

#include "kmr/ds/Queue.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>

namespace kmr::ds {

/**
 * @class CircularArrayQueue
 * @author Kevin Munoz-Rivera
 * @brief FIFO queue backed by a dynamically growing circular array.
 */
template <typename T>
class CircularArrayQueue : public Queue<T> {
public:
    explicit CircularArrayQueue(std::size_t capacity = 16)
        : capacity_(std::max(capacity, std::size_t{1})), data_(std::make_unique<std::optional<T>[]>(capacity_)) {}

    void enqueue(const T& item) override {
        if (size_ == capacity_) grow();
        data_[tail_] = item;
        tail_ = (tail_ + 1) % capacity_;
        ++size_;
    }

    T dequeue() override {
        if (isEmpty()) throw std::underflow_error("dequeue on empty CircularArrayQueue");
        T result = std::move(*data_[head_]);
        data_[head_].reset();
        head_ = (head_ + 1) % capacity_;
        --size_;
        return result;
    }

    const T& front() const override {
        if (isEmpty()) throw std::underflow_error("front on empty CircularArrayQueue");
        return *data_[head_];
    }

    [[nodiscard]] std::size_t size() const noexcept override { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept override { return size_ == 0; }

private:
    std::size_t capacity_;
    std::unique_ptr<std::optional<T>[]> data_;
    std::size_t head_{0};
    std::size_t tail_{0};
    std::size_t size_{0};

    void grow() {
        const std::size_t newCapacity = capacity_ * 2;
        auto replacement = std::make_unique<std::optional<T>[]>(newCapacity);
        for (std::size_t i = 0; i < size_; ++i) {
            replacement[i] = std::move(data_[(head_ + i) % capacity_]);
        }
        data_ = std::move(replacement);
        capacity_ = newCapacity;
        head_ = 0;
        tail_ = size_;
    }
};

}  // namespace kmr::ds
