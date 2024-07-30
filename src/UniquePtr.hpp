#pragma once

#include <compare>
#include <memory>
#include <ostream>
#include <utility>

template <typename T, typename D = std::default_delete<T>>
class UniquePtr {
private:
    T* ptr;
    D deleter;

public:
    explicit UniquePtr(T* rawPtr = nullptr, D d = std::default_delete<T>())
        : ptr(rawPtr), deleter(d) {
    }

    ~UniquePtr() {
        if (ptr) {
            deleter(ptr);
        }
    }

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) {
            deleter(ptr);
            ptr = other.ptr;
            other.ptr = nullptr;
        }
    }

    T* get() {
        return ptr;
    }

    const T* get() const {
        return ptr;
    }

    void swap(UniquePtr<T>& other) {
        std::swap(ptr, other.ptr);
    }

    D& get_deleter() {
        return deleter;
    }

    const D& get_deleter() const {
        return deleter;
    }

    void reset(T* newPtr = nullptr) {
        T* oldPtr = ptr;
        ptr = newPtr;

        if (oldPtr) {
            deleter(oldPtr);
        }
    }

    T* release() {
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

    T* operator->() {
        return ptr;
    }

    const T* operator->() const {
        return ptr;
    }

    explicit operator bool() const {
        return (ptr != nullptr);
    }
};

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

// std::make_unique equivalent
template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
