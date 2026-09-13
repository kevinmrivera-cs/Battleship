#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace kmr::ds {

/**
 * @class MyArrayList
 * @author Kevin Munoz-Rivera
 * @brief Dynamically sized array list implemented with manual C++ storage management.
 *
 * The class preserves the learning goal of the original Java MyArrayList while
 * using RAII and checked access so ownership and bounds errors cannot silently
 * corrupt memory.
 */
template <typename T>
class MyArrayList {
public:
    static constexpr std::size_t DEFAULT_CAPACITY = 16;

    /** @brief Constructs an empty list. */
    MyArrayList() { allocate(DEFAULT_CAPACITY); }

    /** @brief Constructs an empty list with at least the requested capacity. */
    explicit MyArrayList(std::size_t capacity) { allocate(std::max(capacity, std::size_t{1})); }

    /** @brief Copy constructor performing a deep copy. */
    MyArrayList(const MyArrayList& other) { copyFrom(other); }

    /** @brief Move constructor transferring ownership of the backing storage. */
    MyArrayList(MyArrayList&& other) noexcept { moveFrom(std::move(other)); }

    /** @brief Copy assignment using copy-and-swap. */
    MyArrayList& operator=(const MyArrayList& other) {
        if (this != &other) {
            MyArrayList copy(other);
            swap(copy);
        }
        return *this;
    }

    /** @brief Move assignment transferring ownership. */
    MyArrayList& operator=(MyArrayList&& other) noexcept {
        if (this != &other) {
            destroyAll();
            deallocate();
            moveFrom(std::move(other));
        }
        return *this;
    }

    /** @brief Destroys all elements and releases backing storage. */
    ~MyArrayList() {
        destroyAll();
        deallocate();
    }

    /**
     * @brief Inserts an item at the specified index.
     * @param item Item to insert.
     * @param index Position in the inclusive range [0, size()].
     * @throws std::out_of_range if index is invalid.
     */
    void insert(const T& item, std::size_t index) { insertImpl(item, index); }

    /** @overload */
    void insert(T&& item, std::size_t index) { insertImpl(std::move(item), index); }

    /** @brief Appends an item to the end of the list. */
    void add(const T& item) { insert(item, size_); }

    /** @brief Appends a moved item to the end of the list. */
    void add(T&& item) { insert(std::move(item), size_); }

    /**
     * @brief Removes and returns the element at index.
     * @throws std::out_of_range if index is invalid.
     */
    T remove(std::size_t index) {
        validateExistingIndex(index);
        T result = std::move(data_[index]);
        for (std::size_t i = index; i + 1 < size_; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        std::allocator_traits<Allocator>::destroy(allocator_, data_ + (size_ - 1));
        --size_;
        return result;
    }

    /** @brief Returns true when an equal item exists in the list. */
    bool contains(const T& item) const { return indexOf(item) >= 0; }

    /** @brief Returns the zero-based index of item, or -1 when absent. */
    long long indexOf(const T& item) const {
        for (std::size_t i = 0; i < size_; ++i) {
            ++comparisons;
            if (data_[i] == item) {
                return static_cast<long long>(i);
            }
        }
        return -1;
    }

    /** @brief Returns a checked mutable reference to an element. */
    T& get(std::size_t index) { validateExistingIndex(index); return data_[index]; }

    /** @brief Returns a checked const reference to an element. */
    const T& get(std::size_t index) const { validateExistingIndex(index); return data_[index]; }

    /** @brief Returns a pointer to an element or nullptr when index is invalid. */
    T* tryGet(std::size_t index) noexcept { return index < size_ ? data_ + index : nullptr; }

    /** @brief Returns a const pointer to an element or nullptr when index is invalid. */
    const T* tryGet(std::size_t index) const noexcept { return index < size_ ? data_ + index : nullptr; }

    /** @brief Replaces the element at index after validating the index. */
    void set(std::size_t index, const T& item) { validateExistingIndex(index); data_[index] = item; }

    /** @brief Returns the number of stored elements. */
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    /** @brief Returns true when the list has no elements. */
    [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }

    /** @brief Returns the current backing capacity. */
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Sorts elements in ascending order using insertion sort. */
    void sort() {
        for (std::size_t i = 1; i < size_; ++i) {
            std::size_t j = i;
            while (j > 0 && data_[j] < data_[j - 1]) {
                std::swap(data_[j], data_[j - 1]);
                --j;
            }
        }
    }

    /** @brief Returns a Java-course-style string representation. */
    [[nodiscard]] std::string toString() const {
        std::ostringstream out;
        out << '[';
        for (std::size_t i = 0; i < size_; ++i) {
            if (i != 0) out << ", ";
            out << data_[i];
        }
        out << ']';
        return out.str();
    }

    /** @brief Number of equality comparisons made by linear searches. */
    mutable std::size_t comparisons{0};

private:
    using Allocator = std::allocator<T>;
    Allocator allocator_{};
    T* data_{nullptr};
    std::size_t capacity_{0};
    std::size_t size_{0};

    template <typename U>
    void insertImpl(U&& item, std::size_t index) {
        if (index > size_) throw std::out_of_range("MyArrayList insert index out of range");
        if (size_ == capacity_) resize(capacity_ * 2);
        if (index == size_) {
            std::allocator_traits<Allocator>::construct(allocator_, data_ + size_, std::forward<U>(item));
            ++size_;
            return;
        }
        std::allocator_traits<Allocator>::construct(allocator_, data_ + size_, std::move(data_[size_ - 1]));
        for (std::size_t i = size_ - 1; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = std::forward<U>(item);
        ++size_;
    }

    void validateExistingIndex(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("MyArrayList index out of range");
    }

    void allocate(std::size_t capacity) {
        data_ = std::allocator_traits<Allocator>::allocate(allocator_, capacity);
        capacity_ = capacity;
    }

    void resize(std::size_t newCapacity) {
        T* replacement = std::allocator_traits<Allocator>::allocate(allocator_, newCapacity);
        std::size_t built = 0;
        try {
            for (; built < size_; ++built) {
                std::allocator_traits<Allocator>::construct(
                    allocator_, replacement + built, std::move_if_noexcept(data_[built]));
            }
        } catch (...) {
            for (std::size_t i = 0; i < built; ++i) {
                std::allocator_traits<Allocator>::destroy(allocator_, replacement + i);
            }
            std::allocator_traits<Allocator>::deallocate(allocator_, replacement, newCapacity);
            throw;
        }
        destroyAll();
        deallocate();
        data_ = replacement;
        capacity_ = newCapacity;
        size_ = built;
    }

    void copyFrom(const MyArrayList& other) {
        allocate(std::max(other.capacity_, std::size_t{1}));
        try {
            for (; size_ < other.size_; ++size_) {
                std::allocator_traits<Allocator>::construct(allocator_, data_ + size_, other.data_[size_]);
            }
            comparisons = other.comparisons;
        } catch (...) {
            destroyAll();
            deallocate();
            throw;
        }
    }

    void moveFrom(MyArrayList&& other) noexcept {
        data_ = std::exchange(other.data_, nullptr);
        capacity_ = std::exchange(other.capacity_, 0);
        size_ = std::exchange(other.size_, 0);
        comparisons = std::exchange(other.comparisons, 0);
    }

    void destroyAll() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Allocator>::destroy(allocator_, data_ + i);
        }
        size_ = 0;
    }

    void deallocate() noexcept {
        if (data_ != nullptr) {
            std::allocator_traits<Allocator>::deallocate(allocator_, data_, capacity_);
            data_ = nullptr;
            capacity_ = 0;
        }
    }

    void swap(MyArrayList& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(comparisons, other.comparisons);
    }
};

}  // namespace kmr::ds
