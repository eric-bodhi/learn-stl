#include <typeinfo>
#include <cassert>
#include <iostream>

// Simplified std::any implementation
class Any {
private:
    // Function table for type-erased operations
    struct FuncTable {
        void (*destroy)(void*);                // Destructor
        void* (*copy)(void* dest, const void* src); // Copy constructor
        const std::type_info& (*type)();       // Type information
    };

    // Handler for type-specific operations
    template <typename T>
    struct Handler {
        static void destroy(void* obj) {
            static_cast<T*>(obj)->~T(); // Call destructor
        }

        static void* copy(void* dest, const void* src) {
            // Use placement new to copy-construct into destination buffer
            return new (dest) T(*static_cast<const T*>(src));
        }

        static const std::type_info& type() {
            return typeid(T);
        }

        // Static instance of the function table for type T
        static const FuncTable functions;
    };

    // Storage: uses SBO for small objects (adjust size as needed)
    static constexpr size_t BufferSize = 16;
    union {
        void* heap_ptr;                  // For large objects
        std::aligned_storage_t<BufferSize> buffer; // For small objects
    } storage;
    bool on_heap;                        // Track storage location
    const FuncTable* funcs;              // Pointer to function table

public:
    // Constructor for any type T
    template <typename T>
    Any(const T& value) {
        std::cout << "constructed\n";
        using DecayedT = std::decay_t<T>; // Remove references/cv-qualifiers

        if (sizeof(DecayedT) <= BufferSize && alignof(DecayedT) <= alignof(decltype(storage.buffer))) {
            // Store small object in the buffer
            new (&storage.buffer) DecayedT(value);
            on_heap = false;
        } else {
            // Store large object on the heap
            storage.heap_ptr = new DecayedT(value);
            on_heap = true;
        }

        // Set the function table for type T
        funcs = &Handler<DecayedT>::functions;
    }

    // Destructor
    ~Any() {
        std::cout << "destructed\n";
        if (on_heap) {
            funcs->destroy(storage.heap_ptr);
            delete static_cast<char*>(storage.heap_ptr);
        } else {
            funcs->destroy(&storage.buffer);
        }
    }

    // Copy constructor
    Any(const Any& other) : funcs(other.funcs), on_heap(other.on_heap) {
        if (on_heap) {
            // Copy heap-allocated object
            storage.heap_ptr = funcs->copy(nullptr, other.storage.heap_ptr);
        } else {
            // Copy buffer-stored object
            funcs->copy(&storage.buffer, &other.storage.buffer);
        }
    }

    // Get the stored type
    const std::type_info& type() const {
        return funcs->type();
    }

    // Friend function for any_cast
    template <typename T>
    friend T* any_cast(Any* any);
};

// Initialize static function table for each type T
template <typename T>
const Any::FuncTable Any::Handler<T>::functions = {
    &Any::Handler<T>::destroy,
    &Any::Handler<T>::copy,
    &Any::Handler<T>::type
};

// Type-safe retrieval (similar to std::any_cast)
template <typename T>
T* any_cast(Any* any) {
    if (any && any->type() == typeid(T)) {
        if (any->on_heap) {
            return static_cast<T*>(any->storage.heap_ptr);
        } else {
            return reinterpret_cast<T*>(&any->storage.buffer);
        }
    }
    return nullptr;
}

template <typename T>
const T* any_cast(const Any* any) {
    return any_cast<T>(const_cast<Any*>(any));
}

template <typename T>
T any_cast(const Any& any) {
    const T* ptr = any_cast<T>(&any);
    if (!ptr) throw std::bad_cast();
    return *ptr;
}
