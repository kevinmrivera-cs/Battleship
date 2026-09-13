#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace kmr::ds {

/** @brief Collision handling strategy supported by MyHashTable. */
enum class CollisionStrategy { LinearProbing, Chaining };

/**
 * @class MyHashTable
 * @author Kevin Munoz-Rivera
 * @brief Fixed-capacity hash table supporting linear probing or chaining.
 *
 * Probing is explicitly bounded by capacity to prevent infinite loops when a
 * table is full or badly clustered.
 */
template <typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
class MyHashTable {
private:
    struct Entry { Key key; Value value; };
    struct ProbeSlot {
        std::optional<Entry> entry;
        bool tombstone{false};
    };
public:
    explicit MyHashTable(std::size_t capacity = 16,
                         CollisionStrategy strategy = CollisionStrategy::LinearProbing)
        : capacity_(capacity), strategy_(strategy), probe_(capacity), chains_(capacity) {
        if (capacity == 0) throw std::invalid_argument("MyHashTable capacity must be positive");
    }

    /** @brief Inserts or updates a key/value pair. */
    void put(const Key& key, const Value& value) {
        strategy_ == CollisionStrategy::LinearProbing ? putProbe(key, value) : putChain(key, value);
    }

    /** @brief Returns a copy of the value when present. */
    std::optional<Value> get(const Key& key) const {
        return strategy_ == CollisionStrategy::LinearProbing ? getProbe(key) : getChain(key);
    }

    /** @brief Returns true when the key exists. */
    bool containsKey(const Key& key) const { return get(key).has_value(); }

    /** @brief Removes a key/value pair and returns whether it existed. */
    bool remove(const Key& key) {
        return strategy_ == CollisionStrategy::LinearProbing ? removeProbe(key) : removeChain(key);
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }

    mutable std::size_t comparisons{0};
    mutable std::size_t maxProbe{0};

private:
    std::size_t capacity_;
    CollisionStrategy strategy_;
    std::vector<ProbeSlot> probe_;
    std::vector<std::vector<Entry>> chains_;
    std::size_t size_{0};
    Hash hash_{};
    KeyEqual equal_{};

    std::size_t startIndex(const Key& key) const { return hash_(key) % capacity_; }

    void putProbe(const Key& key, const Value& value) {
        const std::size_t start = startIndex(key);
        std::optional<std::size_t> firstTombstone;
        for (std::size_t step = 0; step < capacity_; ++step) {
            const std::size_t index = (start + step) % capacity_;
            ++comparisons;
            maxProbe = std::max(maxProbe, step + 1);
            auto& slot = probe_[index];
            if (slot.entry && equal_(slot.entry->key, key)) {
                slot.entry->value = value;
                return;
            }
            if (!slot.entry) {
                if (slot.tombstone) {
                    if (!firstTombstone) firstTombstone = index;
                    continue;
                }
                const std::size_t target = firstTombstone.value_or(index);
                probe_[target].entry = Entry{key, value};
                probe_[target].tombstone = false;
                ++size_;
                return;
            }
        }
        if (firstTombstone) {
            probe_[*firstTombstone].entry = Entry{key, value};
            probe_[*firstTombstone].tombstone = false;
            ++size_;
            return;
        }
        throw std::overflow_error("MyHashTable linear-probing table is full");
    }

    std::optional<Value> getProbe(const Key& key) const {
        const std::size_t start = startIndex(key);
        for (std::size_t step = 0; step < capacity_; ++step) {
            const std::size_t index = (start + step) % capacity_;
            const auto& slot = probe_[index];
            if (!slot.entry && !slot.tombstone) return std::nullopt;
            if (slot.entry) {
                ++comparisons;
                maxProbe = std::max(maxProbe, step + 1);
                if (equal_(slot.entry->key, key)) return slot.entry->value;
            }
        }
        return std::nullopt;
    }

    bool removeProbe(const Key& key) {
        const std::size_t start = startIndex(key);
        for (std::size_t step = 0; step < capacity_; ++step) {
            const std::size_t index = (start + step) % capacity_;
            auto& slot = probe_[index];
            if (!slot.entry && !slot.tombstone) return false;
            if (slot.entry && equal_(slot.entry->key, key)) {
                slot.entry.reset();
                slot.tombstone = true;
                --size_;
                return true;
            }
        }
        return false;
    }

    void putChain(const Key& key, const Value& value) {
        auto& chain = chains_[startIndex(key)];
        std::size_t probes = 0;
        for (auto& entry : chain) {
            ++probes; ++comparisons;
            if (equal_(entry.key, key)) { entry.value = value; maxProbe = std::max(maxProbe, probes); return; }
        }
        chain.push_back(Entry{key, value});
        ++size_;
        maxProbe = std::max(maxProbe, probes + 1);
    }

    std::optional<Value> getChain(const Key& key) const {
        const auto& chain = chains_[startIndex(key)];
        std::size_t probes = 0;
        for (const auto& entry : chain) {
            ++probes; ++comparisons;
            if (equal_(entry.key, key)) { maxProbe = std::max(maxProbe, probes); return entry.value; }
        }
        maxProbe = std::max(maxProbe, probes);
        return std::nullopt;
    }

    bool removeChain(const Key& key) {
        auto& chain = chains_[startIndex(key)];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (equal_(it->key, key)) {
                chain.erase(it);
                --size_;
                return true;
            }
        }
        return false;
    }
};

}  // namespace kmr::ds
