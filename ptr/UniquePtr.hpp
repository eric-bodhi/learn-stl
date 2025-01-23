#pragma once

#include <iostream>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>

template <typename T, typename D = std::default_delete<T>>
class UniquePtr {
private:
    T* ptr;
    D deleter;

public:
    // Constructor
    explicit UniquePtr(T* rawPtr = nullptr, D d = std::default_delete<T>())
        : ptr(rawPtr), deleter(d) {
    }

    // Destructor
    ~UniquePtr() {
        reset();
    }

    // Deleted copy constructor and assignment operator
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Move constructor
    UniquePtr(UniquePtr&& other) noexcept
        : ptr(other.ptr), deleter(std::move(other.deleter)) {
        other.ptr = nullptr;
    }

    // Move assignment operator
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr = other.ptr;
            deleter = std::move(other.deleter);
            other.ptr = nullptr;
        }
        return *this;
    }

    // Member functions
    T* get() noexcept {
        return ptr;
    }

    const T* get() const noexcept {
        return ptr;
    }

    D& get_deleter() noexcept {
        return deleter;
    }

    const D& get_deleter() const noexcept {
        return deleter;
    }

    void reset(T* newPtr = nullptr) {
        T* oldPtr = ptr;
        ptr = newPtr;
        if (oldPtr) {
            deleter(oldPtr);
        }
    }

    T* release() noexcept {
        T* retPtr = ptr;
        ptr = nullptr;
        return retPtr;
    }

    T& operator*() {
        return *ptr;
    }

    const T& operator*() const {
        return *ptr;
    }

    T* operator->() noexcept {
        return ptr;
    }

    const T* operator->() const noexcept {
        return ptr;
    }

    explicit operator bool() const noexcept {
        return (ptr != nullptr);
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(deleter, other.deleter);
    }
};

// Non-member functions for comparison
template <class T1, class D1, class T2, class D2>
bool operator==(const UniquePtr<T1, D1>& x, const UniquePtr<T2, D2>& y) {
    return x.get() == y.get();
}

template <class T1, class D1, class T2, class D2>
bool operator!=(const UniquePtr<T1, D1>& x, const UniquePtr<T2, D2>& y) {
    return !(x == y);
}

template <class T1, class D1, class T2, class D2>
bool operator<(const UniquePtr<T1, D1>& x, const UniquePtr<T2, D2>& y) {
    return x.get() < y.get();
}

template <class T1, class D1, class T2, class D2>
bool operator>(const UniquePtr<T1, D1>& x, const UniquePtr<T2, D2>& y) {
    return y < x;
}

template <class T1, class D1, class T2, class D2>
bool operator<=(const UniquePtr<T1, D1>& x, const UniquePtr<T2, D2>& y) {
    return !(x > y);
}

template <class T1, class D1, class T2, class D2>
bool operator>=(const UniquePtr<T1, D1>& x, const UniquePtr<T2, D2>& y) {
    return !(x < y);
}

// Output stream operator
template <typename CharT, typename Traits, typename T, typename D>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const UniquePtr<T, D>& p) {
    if (p) {
        os << *p;
    } else {
        os << "null";
    }
    return os;
}

// Singular std::make_unique equivalent
template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args)
    requires(!std::is_array_v<T>)
{
    std::cout << "Non-array version\n";
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename D>
class UniquePtr<T[], D> {
private:
    T* ptr;
    D deleter;

public:
    explicit UniquePtr(size_t n = 0, D d = std::default_delete<T[]>())
        : ptr(new T[n]), deleter(d) {
    }

    ~UniquePtr() {
        reset();
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Move constructor
    UniquePtr(UniquePtr&& other) noexcept
        : ptr(other.ptr), deleter(std::move(other.deleter)) {
        other.ptr = nullptr;
    }

    // Move assignment operator
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr = other.ptr;
            deleter = std::move(other.deleter);
            other.ptr = nullptr;
        }
        return *this;
    }

    // Member functions
    T* get() noexcept {
        return ptr;
    }

    const T* get() const noexcept {
        return ptr;
    }

    D& get_deleter() noexcept {
        return deleter;
    }

    const D& get_deleter() const noexcept {
        return deleter;
    }

    void reset(T* newPtr = nullptr) {
        T* oldPtr = ptr;
        ptr = newPtr;
        if (oldPtr) {
            deleter(oldPtr);
        }
    }

    T* release() noexcept {
        T* retPtr = ptr;
        ptr = nullptr;
        return retPtr;
    }

    T& operator[](size_t n) const {
        return ptr[n];
    }

    T& operator*() {
        return *ptr;
    }

    const T& operator*() const {
        return *ptr;
    }

    T* operator->() noexcept {
        return ptr;
    }

    const T* operator->() const noexcept {
        return ptr;
    }

    explicit operator bool() const noexcept {
        return (ptr != nullptr);
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(deleter, other.deleter);
    }
};

template <class T, class D>
bool operator==(const UniquePtr<T[], D>& x, std::nullptr_t) noexcept {
    return (*x == nullptr);
}

template <typename T>
UniquePtr<T> makeUnique(size_t n)
    requires(std::is_array_v<T>)
{
    std::cout << "Array version\n";
    using ElementType = std::remove_extent_t<T>;
    return UniquePtr<ElementType[]>(n);
}

template <typename T, std::size_t N>
UniquePtr<T> makeUnique()
    requires(std::is_array_v<T>)
{
    std::cout << "Array with known bounds \n";
    using ElementType = std::remove_extent_t<T>;
    return UniquePtr<ElementType[]>(N);
}