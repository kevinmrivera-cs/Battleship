#pragma once

#include "kmr/ds/Queue.h"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace kmr::ds {

/**
 * @class LinkedQueue
 * @author Kevin Munoz-Rivera
 * @brief Linked FIFO queue with O(1) enqueue and dequeue operations.
 */
template <typename T>
class LinkedQueue : public Queue<T> {
private:
    struct Node {
        T value;
        std::unique_ptr<Node> next;
        explicit Node(const T& item) : value(item) {}
    };
public:
    void enqueue(const T& item) override {
        auto node = std::make_unique<Node>(item);
        Node* raw = node.get();
        if (tail_ == nullptr) head_ = std::move(node);
        else tail_->next = std::move(node);
        tail_ = raw;
        ++size_;
    }

    T dequeue() override {
        if (isEmpty()) throw std::underflow_error("dequeue on empty LinkedQueue");
        auto old = std::move(head_);
        head_ = std::move(old->next);
        --size_;
        if (size_ == 0) tail_ = nullptr;
        return std::move(old->value);
    }

    const T& front() const override {
        if (isEmpty()) throw std::underflow_error("front on empty LinkedQueue");
        return head_->value;
    }

    [[nodiscard]] std::size_t size() const noexcept override { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept override { return size_ == 0; }

private:
    std::unique_ptr<Node> head_;
    Node* tail_{nullptr};
    std::size_t size_{0};
};

}  // namespace kmr::ds
