#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace kmr::ds {

/**
 * @class LinkedBinaryTree
 * @author Kevin Munoz-Rivera
 * @brief Generic linked binary tree with safe ownership and traversals.
 */
template <typename T>
class LinkedBinaryTree {
public:
    struct Node {
        T value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(const T& item) : value(item) {}
    };

    /** @brief Replaces the tree with a new root value. */
    Node& setRoot(const T& item) {
        root_ = std::make_unique<Node>(item);
        size_ = 1;
        return *root_;
    }

    /** @brief Attaches/replaces a left child. */
    Node& setLeft(Node& parent, const T& item) {
        if (parent.left) throw std::logic_error("Left child already exists");
        parent.left = std::make_unique<Node>(item);
        ++size_;
        return *parent.left;
    }

    /** @brief Attaches a right child; refuses to silently destroy an existing subtree. */
    Node& setRight(Node& parent, const T& item) {
        if (parent.right) throw std::logic_error("Right child already exists");
        parent.right = std::make_unique<Node>(item);
        ++size_;
        return *parent.right;
    }

    Node* root() noexcept { return root_.get(); }
    const Node* root() const noexcept { return root_.get(); }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }

    /** @brief Returns an inorder traversal. */
    [[nodiscard]] std::vector<T> inOrder() const {
        std::vector<T> output;
        output.reserve(size_);
        walkInOrder(root_.get(), output);
        return output;
    }

private:
    std::unique_ptr<Node> root_;
    std::size_t size_{0};

    static void walkInOrder(const Node* node, std::vector<T>& output) {
        if (!node) return;
        walkInOrder(node->left.get(), output);
        output.push_back(node->value);
        walkInOrder(node->right.get(), output);
    }
};

}  // namespace kmr::ds
