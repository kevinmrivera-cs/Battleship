#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace kmr::ds {

/**
 * @class MyLinkedList
 * @author Kevin Munoz-Rivera
 * @brief Singly linked list with safe ownership and a course-style traversal cursor.
 */
template <typename T>
class MyLinkedList {
private:
    struct Node {
        T value;
        std::unique_ptr<Node> next;
        explicit Node(const T& item) : value(item) {}
        explicit Node(T&& item) : value(std::move(item)) {}
    };

public:
    MyLinkedList() = default;
    MyLinkedList(const MyLinkedList& other) { copyFrom(other); }
    MyLinkedList(MyLinkedList&&) noexcept = default;
    MyLinkedList& operator=(MyLinkedList other) noexcept { swap(other); return *this; }
    ~MyLinkedList() = default;

    /** @brief Appends an item to the tail in O(1) time. */
    void add(const T& item) { appendNode(std::make_unique<Node>(item)); }
    void add(T&& item) { appendNode(std::make_unique<Node>(std::move(item))); }

    /** @brief Inserts an item at index. */
    void insert(const T& item, std::size_t index) { insertNode(std::make_unique<Node>(item), index); }

    /** @brief Removes and returns the item at index. */
    T remove(std::size_t index) {
        if (index >= size_) throw std::out_of_range("MyLinkedList index out of range");
        if (index == 0) {
            auto old = std::move(head_);
            head_ = std::move(old->next);
            --size_;
            if (size_ == 0) tail_ = nullptr;
            cursor_ = nullptr;
            return std::move(old->value);
        }
        Node* prev = nodeAt(index - 1);
        auto old = std::move(prev->next);
        prev->next = std::move(old->next);
        if (index == size_ - 1) tail_ = prev;
        --size_;
        cursor_ = nullptr;
        return std::move(old->value);
    }

    /** @brief Returns checked mutable access to index. */
    T& get(std::size_t index) { return nodeAt(index)->value; }
    /** @brief Returns checked const access to index. */
    const T& get(std::size_t index) const { return nodeAt(index)->value; }

    /** @brief Updates an existing element. */
    void set(std::size_t index, const T& item) { nodeAt(index)->value = item; }

    /** @brief Returns true if an equal item exists. */
    bool contains(const T& item) const { return indexOf(item) >= 0; }

    /** @brief Returns item index or -1 when absent. */
    long long indexOf(const T& item) const {
        std::size_t index = 0;
        for (Node* n = head_.get(); n != nullptr; n = n->next.get(), ++index) {
            ++comparisons;
            if (n->value == item) return static_cast<long long>(index);
        }
        return -1;
    }

    /** @brief Resets the traversal cursor to the first node. */
    void first() noexcept { cursor_ = head_.get(); }
    /** @brief Advances the traversal cursor one node. */
    void next() noexcept { if (cursor_ != nullptr) cursor_ = cursor_->next.get(); }
    /** @brief Returns a pointer to the current cursor value or nullptr at end. */
    T* current() noexcept { return cursor_ != nullptr ? &cursor_->value : nullptr; }
    const T* current() const noexcept { return cursor_ != nullptr ? &cursor_->value : nullptr; }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }

    [[nodiscard]] std::string toString() const {
        std::ostringstream out;
        out << '[';
        bool firstItem = true;
        for (Node* n = head_.get(); n != nullptr; n = n->next.get()) {
            if (!firstItem) out << ", ";
            out << n->value;
            firstItem = false;
        }
        out << ']';
        return out.str();
    }

    mutable std::size_t comparisons{0};

private:
    std::unique_ptr<Node> head_;
    Node* tail_{nullptr};
    Node* cursor_{nullptr};
    std::size_t size_{0};

    Node* nodeAt(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("MyLinkedList index out of range");
        Node* n = head_.get();
        for (std::size_t i = 0; i < index; ++i) n = n->next.get();
        return n;
    }

    void appendNode(std::unique_ptr<Node> node) {
        Node* raw = node.get();
        if (tail_ == nullptr) head_ = std::move(node);
        else tail_->next = std::move(node);
        tail_ = raw;
        ++size_;
    }

    void insertNode(std::unique_ptr<Node> node, std::size_t index) {
        if (index > size_) throw std::out_of_range("MyLinkedList insert index out of range");
        if (index == size_) { appendNode(std::move(node)); return; }
        if (index == 0) {
            node->next = std::move(head_);
            head_ = std::move(node);
            if (tail_ == nullptr) tail_ = head_.get();
            ++size_;
            cursor_ = nullptr;
            return;
        }
        Node* prev = nodeAt(index - 1);
        node->next = std::move(prev->next);
        prev->next = std::move(node);
        ++size_;
        cursor_ = nullptr;
    }

    void copyFrom(const MyLinkedList& other) {
        for (Node* n = other.head_.get(); n != nullptr; n = n->next.get()) add(n->value);
        comparisons = other.comparisons;
    }

    void swap(MyLinkedList& other) noexcept {
        using std::swap;
        swap(head_, other.head_);
        swap(tail_, other.tail_);
        swap(size_, other.size_);
        swap(comparisons, other.comparisons);
        cursor_ = nullptr;
        other.cursor_ = nullptr;
    }
};

}  // namespace kmr::ds
