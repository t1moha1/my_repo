#ifndef VECTOR_H
#define VECTOR_H
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <utility>

class VectorInitializationException : public std::exception {
public:
    explicit VectorInitializationException(std::string  message)
            : message_(std::move(message)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};

template <typename T>
class Vector {
public:
    Vector() : size_(0), capacity_(0), data_(nullptr) {}

    explicit Vector(size_t size) : size_(size), capacity_(size) {
        if (size_ > 0) {
            data_ = new T[size_];
            try {
                for (size_t i = 0; i < size_; i++) {
                    data_[i] = T();
                }
            } catch (...) {
                delete[] data_;
                throw VectorInitializationException("Error initializing vector");
            }
        } else {
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }
    }

    Vector(size_t size, T value) : size_(size), capacity_(size) {
        if (size_ > 0) {
            data_ = new T[size_];
            try {
                std::fill(data_, data_ + size, value);
            } catch (...) {
                delete[] data_;
                throw VectorInitializationException("Error initializing vector");
            }
        } else {
            data_ = nullptr;
            size_ = 0;
        }
    }

    Vector(std::initializer_list<T> list) : size_(list.size()), capacity_(list.size()) {
        if (size_ != 0) {
            data_ = new T[size_];
            try {
                std::copy(list.begin(), list.end(), data_);
            } catch (...) {
                delete[] data_;
                throw VectorInitializationException("Error initializing vector");
            }
        } else {
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }
    }

    ~Vector() {
        delete[] data_;
    }

    Vector(const Vector& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        if (size_ != 0) {
            data_ = new T[size_];
            try {
                std::copy(other.data_, other.data_ + other.size_, data_);
            } catch (...) {
                delete[] data_;
                throw VectorInitializationException("Error initializing vector");
            }
        } else {
            data_ = nullptr;
        }
    }

    Vector(Vector&& other) noexcept : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            data_ = nullptr;
            if (size_ != 0) {
                data_ = new T[size_];
            }
            std::copy(other.data_, other.data_ + size_, data_);
        }
        return *this;
    }

    T& operator[](size_t idx) {
        return data_[idx];
    }

    const T& operator[](size_t idx) const {
        return data_[idx];
    }

    T& At(size_t idx) {
        if (idx >= size_) {
            throw std::out_of_range("");
        }
        return data_[idx];
    }

    [[nodiscard]] const T& At(size_t idx) const {
        if (idx >= size_) {
            throw std::out_of_range("");
        }
        return data_[idx];
    }

    T& Front() {
        return data_[0];
    }

    [[nodiscard]] const T& Front() const {
        return data_[0];
    }

    T& Back() {
        return data_[size_ - 1];
    }

    [[nodiscard]] const T& Back() const {
        return data_[size_ - 1];
    }

    T* Data() {
        return data_;
    }

    [[nodiscard]] const T* Data() const {
        return data_;
    }

    [[nodiscard]] size_t Size() const {
        return size_;
    }

    [[nodiscard]] size_t Capacity() const {
        return capacity_;
    }

    [[nodiscard]] bool Empty() const {
        return (size_ == 0);
    }

    void Swap(Vector& arr) noexcept {
        std::swap(size_, arr.size_);
        std::swap(capacity_, arr.capacity_);
        std::swap(data_, arr.data_);
    }

    void Resize(size_t new_size) {
        if (new_size <= 0) {
            delete[] data_;
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        } else if (new_size <= size_) {
            size_ = new_size;
        } else {
            if (new_size > capacity_) {
                Reserve(new_size);
            }
            for (size_t i = size_; i < new_size; i++) {
                data_[i] = T();
            }
            size_ = new_size;
        }
    }

    void Resize(size_t new_size, T value) {
        if (new_size <= 0) {
            delete[] data_;
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
            return;
        }
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        if (new_size > capacity_) {
            Reserve(new_size);
        }
        for (size_t i = size_; i < new_size; i++) {
            data_[i] = std::move(value);
        }
        size_ = new_size;
    }

    void ShrinkToFit() {
        if (capacity_ > size_) {
            T* new_data = size_ ? new T[size_] : nullptr;
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = std::move(data_[i]);
            }
            delete[] data_;
            data_ = new_data;
            capacity_ = size_;
        }
    }

    void Clear() {
        size_ = 0;
    }

    void Reserve(size_t new_cap) {
        if (new_cap == capacity_ || new_cap < capacity_) {
            return;
        }
        if (new_cap == 0) {
            delete[] data_;
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
            return;
        }
        if (capacity_ == 0) {
            data_ = new T[new_cap];
            capacity_ = new_cap;
            return;
        }
        if (size_ == 0) {
            delete[] data_;
            data_ = new T[new_cap];
            capacity_ = new_cap;
        }
        auto tmp = new T[new_cap];
        for (size_t i = 0; i < size_; i++) {
            tmp[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = tmp;
        capacity_ = new_cap;
    }

    void PushBack(const T& value) {
        auto tmp_old = data_[size_ - 1];
        try {
            data_[size_ - 1] = value;
        } catch (...) {
            throw;
        }
        data_[size_ - 1] = tmp_old;
        if (capacity_ == 0) {
            Reserve(1);
        }
        if (size_ == capacity_) {
            Reserve(capacity_ * 2);
        }
        try {
            data_[size_] = value;
            size_++;
        } catch (...) {
            throw;
        }
    }

    void PushBack(T&& value) {
        if (capacity_ == 0) {
            Reserve(1);
        }
        if (size_ == capacity_) {
            Reserve(capacity_ * 2);
        }
        data_[size_] = std::move(value);
        size_++;
    }

    bool PopBack() {
        if (this->Empty()) {
            return false;
        }
        size_--;
        return true;
    }

    bool operator==(const Vector& arr) const {
        if (size_ != arr.size_) {
            return false;
        }
        return std::equal(data_, data_ + size_, arr.data_);
    }

    bool operator!=(const Vector& arr) const {
        return *this != arr;
    }

    bool operator<(const Vector& arr) const {
        return std::lexicographical_compare(data_, data_ + size_, arr.data_, arr.data_ + arr.size_);
    }

    bool operator>(const Vector& arr) const {
        return arr < *this;
    }

    bool operator<=(const Vector& arr) const {
        return arr >= *this;
    }

    bool operator>=(const Vector& arr) const {
        return *this >= arr;
    }

    T* begin() noexcept {  // NOLINT
        return data_;
    }

    const T* begin() const noexcept {  // NOLINT
        return data_;
    }

    const T* cbegin() const noexcept {  // NOLINT
        return data_;
    }

    T* end() noexcept {  // NOLINT
        return data_ + size_;
    }

    const T* end() const noexcept {  // NOLINT
        return data_ + size_;
    }

    const T* cend() const noexcept {  // NOLINT
        return data_ + size_;
    }

    std::reverse_iterator<T*> rbegin() noexcept {  // NOLINT
        return std::reverse_iterator<T*>(end());
    }

    std::reverse_iterator<const T*> rbegin() const noexcept {  // NOLINT
        return std::reverse_iterator<const T*>(end());
    }

    std::reverse_iterator<const T*> crbegin() const noexcept {  // NOLINT
        return std::reverse_iterator<const T*>(end());
    }

    std::reverse_iterator<T*> rend() noexcept {  // NOLINT
        return std::reverse_iterator<T*>(begin());
    }

    std::reverse_iterator<const T*> rend() const noexcept {  // NOLINT
        return std::reverse_iterator<const T*>(begin());
    }

    std::reverse_iterator<const T*> crend() const noexcept {  // NOLINT
        return std::reverse_iterator<const T*>(begin());
    }

private:
    size_t size_;
    size_t capacity_;
    T* data_;
};
#endif  // VECTOR_H



