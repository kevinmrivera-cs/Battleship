#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace kmr::ds {

/**
 * @class MyTrie
 * @author Kevin Munoz-Rivera
 * @brief Prefix tree for storing unique strings.
 */
class MyTrie {
private:
    struct Node {
        bool terminal{false};
        std::map<char, std::unique_ptr<Node>> children;
    };
public:
    /** @brief Inserts a word. Returns false when the word already exists. */
    bool insert(const std::string& word) {
        Node* node = &root_;
        for (char ch : word) {
            ++comparisons;
            auto& child = node->children[ch];
            if (!child) child = std::make_unique<Node>();
            node = child.get();
        }
        if (node->terminal) return false;
        node->terminal = true;
        ++size_;
        return true;
    }

    /** @brief Returns true when the exact word is present. */
    bool find(const std::string& word) const {
        const Node* node = &root_;
        for (char ch : word) {
            ++comparisons;
            auto it = node->children.find(ch);
            if (it == node->children.end()) return false;
            node = it->second.get();
        }
        return node->terminal;
    }

    /** @brief Removes a stored word without removing shared prefixes. */
    bool remove(const std::string& word) {
        bool removed = false;
        removeRecursive(root_, word, 0, removed);
        if (removed) --size_;
        return removed;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::vector<std::string> words() const { std::vector<std::string> out; std::string prefix; collect(root_, prefix, out); return out; }

    mutable std::size_t comparisons{0};

private:
    Node root_;
    std::size_t size_{0};

    static bool removeRecursive(Node& node, const std::string& word, std::size_t depth, bool& removed) {
        if (depth == word.size()) {
            if (!node.terminal) return false;
            node.terminal = false;
            removed = true;
            return node.children.empty();
        }
        auto it = node.children.find(word[depth]);
        if (it == node.children.end()) return false;
        if (removeRecursive(*it->second, word, depth + 1, removed)) node.children.erase(it);
        return !node.terminal && node.children.empty();
    }

    static void collect(const Node& node, std::string& prefix, std::vector<std::string>& out) {
        if (node.terminal) out.push_back(prefix);
        for (const auto& [ch, child] : node.children) {
            prefix.push_back(ch);
            collect(*child, prefix, out);
            prefix.pop_back();
        }
    }
};

}  // namespace kmr::ds
