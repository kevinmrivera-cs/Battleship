#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace kmr::ds {

/**
 * @class MyBinarySearchTree
 * @author Kevin Munoz-Rivera
 * @brief Unbalanced binary search tree supporting insert, find, and predecessor-based removal.
 */
template <typename T, typename Compare = std::less<T>>
class MyBinarySearchTree {
private:
    struct Node {
        T value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(const T& item) : value(item) {}
    };
public:
    /** @brief Inserts a unique value. Returns false for duplicates. */
    bool add(const T& item) { return addNode(root_, item); }

    /** @brief Finds a stored value and returns a pointer to it, or nullptr. */
    const T* find(const T& item) const {
        Node* node = root_.get();
        while (node) {
            ++comparisons;
            if (compare_(item, node->value)) node = node->left.get();
            else if (compare_(node->value, item)) node = node->right.get();
            else return &node->value;
        }
        return nullptr;
    }

    /** @brief Removes an item. A two-child removal uses the inorder predecessor. */
    bool remove(const T& item) { return removeNode(root_, item); }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }

    [[nodiscard]] std::vector<T> inOrder() const {
        std::vector<T> values;
        values.reserve(size_);
        traverse(root_.get(), values);
        return values;
    }

    mutable std::size_t comparisons{0};

private:
    std::unique_ptr<Node> root_;
    std::size_t size_{0};
    Compare compare_{};

    bool addNode(std::unique_ptr<Node>& node, const T& item) {
        if (!node) {
            node = std::make_unique<Node>(item);
            ++size_;
            return true;
        }
        ++comparisons;
        if (compare_(item, node->value)) return addNode(node->left, item);
        if (compare_(node->value, item)) return addNode(node->right, item);
        return false;
    }

    bool removeNode(std::unique_ptr<Node>& node, const T& item) {
        if (!node) return false;
        ++comparisons;
        if (compare_(item, node->value)) return removeNode(node->left, item);
        if (compare_(node->value, item)) return removeNode(node->right, item);

        if (!node->left) node = std::move(node->right);
        else if (!node->right) node = std::move(node->left);
        else {
            Node* predecessor = node->left.get();
            while (predecessor->right) predecessor = predecessor->right.get();
            node->value = predecessor->value;
            removeNode(node->left, predecessor->value);
            ++size_; // compensate for nested decrement; outer deletion happens once
        }
        --size_;
        return true;
    }

    static void traverse(const Node* node, std::vector<T>& values) {
        if (!node) return;
        traverse(node->left.get(), values);
        values.push_back(node->value);
        traverse(node->right.get(), values);
    }
};

}  // namespace kmr::ds
