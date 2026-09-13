#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

namespace kmr::ds {

/**
 * @class RedBlackTree
 * @author Kevin Munoz-Rivera
 * @brief Left-leaning red-black search tree with balanced insert, search, and removal.
 *
 * The implementation keeps the root black and uses color flips and rotations
 * to maintain the red-black invariants. Ownership is represented with
 * std::unique_ptr so nodes cannot leak or be double-freed.
 */
template <typename T, typename Compare = std::less<T>>
class RedBlackTree {
private:
    enum class Color { Red, Black };
    struct Node {
        T value;
        Color color{Color::Red};
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(const T& item) : value(item) {}
    };

public:
    /** @brief Inserts a unique value and returns false for a duplicate. */
    bool add(const T& item) {
        bool inserted = false;
        root_ = insert(std::move(root_), item, inserted);
        if (root_) root_->color = Color::Black;
        if (inserted) ++size_;
        return inserted;
    }

    /** @brief Returns a pointer to the stored item or nullptr when absent. */
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

    /** @brief Removes a value while preserving red-black balance. */
    bool remove(const T& item) {
        if (!root_ || find(item) == nullptr) return false;
        if (!isRed(root_->left) && !isRed(root_->right)) root_->color = Color::Red;
        root_ = erase(std::move(root_), item);
        if (root_) root_->color = Color::Black;
        --size_;
        return true;
    }

    /** @brief Returns the number of stored values. */
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    /** @brief Returns true when the tree has no values. */
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }
    /** @brief Returns the values in ascending order. */
    [[nodiscard]] std::vector<T> inOrder() const {
        std::vector<T> out;
        out.reserve(size_);
        traverse(root_.get(), out);
        return out;
    }

    mutable std::size_t comparisons{0};
    std::size_t rotations{0};

private:
    std::unique_ptr<Node> root_;
    std::size_t size_{0};
    Compare compare_{};

    static bool isRed(const std::unique_ptr<Node>& node) {
        return node && node->color == Color::Red;
    }

    std::unique_ptr<Node> rotateLeft(std::unique_ptr<Node> h) {
        auto x = std::move(h->right);
        h->right = std::move(x->left);
        x->left = std::move(h);
        x->color = x->left->color;
        x->left->color = Color::Red;
        ++rotations;
        return x;
    }

    std::unique_ptr<Node> rotateRight(std::unique_ptr<Node> h) {
        auto x = std::move(h->left);
        h->left = std::move(x->right);
        x->right = std::move(h);
        x->color = x->right->color;
        x->right->color = Color::Red;
        ++rotations;
        return x;
    }

    static void flipColors(Node& h) {
        h.color = (h.color == Color::Red) ? Color::Black : Color::Red;
        if (h.left) h.left->color = (h.left->color == Color::Red) ? Color::Black : Color::Red;
        if (h.right) h.right->color = (h.right->color == Color::Red) ? Color::Black : Color::Red;
    }

    std::unique_ptr<Node> moveRedLeft(std::unique_ptr<Node> h) {
        flipColors(*h);
        if (h->right && isRed(h->right->left)) {
            h->right = rotateRight(std::move(h->right));
            h = rotateLeft(std::move(h));
            flipColors(*h);
        }
        return h;
    }

    std::unique_ptr<Node> moveRedRight(std::unique_ptr<Node> h) {
        flipColors(*h);
        if (h->left && isRed(h->left->left)) {
            h = rotateRight(std::move(h));
            flipColors(*h);
        }
        return h;
    }

    std::unique_ptr<Node> fixUp(std::unique_ptr<Node> h) {
        if (isRed(h->right)) h = rotateLeft(std::move(h));
        if (isRed(h->left) && h->left && isRed(h->left->left)) h = rotateRight(std::move(h));
        if (isRed(h->left) && isRed(h->right)) flipColors(*h);
        return h;
    }

    std::unique_ptr<Node> insert(std::unique_ptr<Node> h, const T& item, bool& inserted) {
        if (!h) {
            inserted = true;
            return std::make_unique<Node>(item);
        }
        ++comparisons;
        if (compare_(item, h->value)) h->left = insert(std::move(h->left), item, inserted);
        else if (compare_(h->value, item)) h->right = insert(std::move(h->right), item, inserted);
        else return h;

        if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(std::move(h));
        if (isRed(h->left) && h->left && isRed(h->left->left)) h = rotateRight(std::move(h));
        if (isRed(h->left) && isRed(h->right)) flipColors(*h);
        return h;
    }

    static Node* minNode(Node* node) {
        while (node->left) node = node->left.get();
        return node;
    }

    std::unique_ptr<Node> deleteMin(std::unique_ptr<Node> h) {
        if (!h->left) return nullptr;
        if (!isRed(h->left) && h->left && !isRed(h->left->left)) h = moveRedLeft(std::move(h));
        h->left = deleteMin(std::move(h->left));
        return fixUp(std::move(h));
    }

    std::unique_ptr<Node> erase(std::unique_ptr<Node> h, const T& item) {
        if (compare_(item, h->value)) {
            if (h->left) {
                if (!isRed(h->left) && !isRed(h->left->left)) h = moveRedLeft(std::move(h));
                h->left = erase(std::move(h->left), item);
            }
        } else {
            if (isRed(h->left)) h = rotateRight(std::move(h));
            const bool equal = !compare_(item, h->value) && !compare_(h->value, item);
            if (equal && !h->right) return nullptr;
            if (h->right) {
                if (!isRed(h->right) && !isRed(h->right->left)) h = moveRedRight(std::move(h));
                const bool equalAfterMove = !compare_(item, h->value) && !compare_(h->value, item);
                if (equalAfterMove) {
                    Node* successor = minNode(h->right.get());
                    h->value = successor->value;
                    h->right = deleteMin(std::move(h->right));
                } else {
                    h->right = erase(std::move(h->right), item);
                }
            }
        }
        return fixUp(std::move(h));
    }

    static void traverse(const Node* node, std::vector<T>& out) {
        if (!node) return;
        traverse(node->left.get(), out);
        out.push_back(node->value);
        traverse(node->right.get(), out);
    }
};

}  // namespace kmr::ds
