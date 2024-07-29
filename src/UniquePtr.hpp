#pragma once

#include <iostream>

template <typename T>
class UniquePtr {
private:
    T* ptr;

public:
    explicit UniquePtr(T* rawPtr = nullptr) : ptr(rawPtr) {
        std::cout << "Ptr Constructed\n";
    }

    ~UniquePtr() {
        delete ptr;
        std::cout << "Ptr deconstructed \n";
    }

    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
            std::cout << "Moved assignment \n";
        }
    }

    T& operator*() {
        return *ptr;
    }
};

template <typename T>
UniquePtr<T> makeUnique(T arg) {
    return UniquePtr(new T(arg));
}
