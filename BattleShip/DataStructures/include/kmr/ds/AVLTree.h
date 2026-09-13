#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

namespace kmr::ds {

/**
 * @class AVLTree
 * @author Kevin Munoz-Rivera
 * @brief Self-balancing binary search tree using AVL rotations.
 */
template <typename T, typename Compare = std::less<T>>
class AVLTree {
private:
    struct Node {
        T value;
        int height{0};
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(const T& item) : value(item) {}
    };
public:
    bool add(const T& item) {
        bool inserted = false;
        root_ = insert(std::move(root_), item, inserted);
        if (inserted) ++size_;
        return inserted;
    }

    bool remove(const T& item) {
        bool removed = false;
        root_ = erase(std::move(root_), item, removed);
        if (removed) --size_;
        return removed;
    }

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

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }
    [[nodiscard]] int height() const noexcept { return nodeHeight(root_.get()); }
    [[nodiscard]] std::vector<T> inOrder() const { std::vector<T> out; traverse(root_.get(), out); return out; }

    mutable std::size_t comparisons{0};
    std::size_t rotations{0};

private:
    std::unique_ptr<Node> root_;
    std::size_t size_{0};
    Compare compare_{};

    static int nodeHeight(const Node* n) noexcept { return n ? n->height : -1; }
    static void update(Node* n) { n->height = 1 + std::max(nodeHeight(n->left.get()), nodeHeight(n->right.get())); }
    static int balanceFactor(const Node* n) { return nodeHeight(n->left.get()) - nodeHeight(n->right.get()); }

    std::unique_ptr<Node> rotateRight(std::unique_ptr<Node> y) {
        auto x = std::move(y->left);
        y->left = std::move(x->right);
        x->right = std::move(y);
        update(x->right.get());
        update(x.get());
        ++rotations;
        return x;
    }

    std::unique_ptr<Node> rotateLeft(std::unique_ptr<Node> x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        y->left = std::move(x);
        update(y->left.get());
        update(y.get());
        ++rotations;
        return y;
    }

    std::unique_ptr<Node> rebalance(std::unique_ptr<Node> n) {
        update(n.get());
        const int bf = balanceFactor(n.get());
        if (bf > 1) {
            if (balanceFactor(n->left.get()) < 0) n->left = rotateLeft(std::move(n->left));
            return rotateRight(std::move(n));
        }
        if (bf < -1) {
            if (balanceFactor(n->right.get()) > 0) n->right = rotateRight(std::move(n->right));
            return rotateLeft(std::move(n));
        }
        return n;
    }

    std::unique_ptr<Node> insert(std::unique_ptr<Node> node, const T& item, bool& inserted) {
        if (!node) { inserted = true; return std::make_unique<Node>(item); }
        ++comparisons;
        if (compare_(item, node->value)) node->left = insert(std::move(node->left), item, inserted);
        else if (compare_(node->value, item)) node->right = insert(std::move(node->right), item, inserted);
        else return node;
        return rebalance(std::move(node));
    }

    std::unique_ptr<Node> erase(std::unique_ptr<Node> node, const T& item, bool& removed) {
        if (!node) return nullptr;
        ++comparisons;
        if (compare_(item, node->value)) node->left = erase(std::move(node->left), item, removed);
        else if (compare_(node->value, item)) node->right = erase(std::move(node->right), item, removed);
        else {
            removed = true;
            if (!node->left) return std::move(node->right);
            if (!node->right) return std::move(node->left);
            Node* predecessor = node->left.get();
            while (predecessor->right) predecessor = predecessor->right.get();
            node->value = predecessor->value;
            bool ignored = false;
            node->left = erase(std::move(node->left), predecessor->value, ignored);
        }
        return node ? rebalance(std::move(node)) : nullptr;
    }

    static void traverse(const Node* node, std::vector<T>& out) {
        if (!node) return;
        traverse(node->left.get(), out);
        out.push_back(node->value);
        traverse(node->right.get(), out);
    }
};

}  // namespace kmr::ds
