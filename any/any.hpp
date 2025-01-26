#include <cassert>
#include <cstddef>
#include <type_traits>
#include <typeinfo>
#include <new>

class Any {
private:
    struct FuncTable {
        void (*destroy)(void*);
        void* (*copy)(void* dest, const void* src);
        const std::type_info& (*type)();
    };

    template <typename T>
    struct Handler {
        static void destroy(void* obj) {
            static_cast<T*>(obj)->~T();
        }

        static void* copy(void* dest, const void* src) {
            return new (dest) T(*static_cast<const T*>(src));
        }

        static const std::type_info& type() {
            return typeid(T);
        }

        static const FuncTable functions;
    };

    static constexpr std::size_t BufferSize = 16;
    static constexpr std::size_t BufferAlignment = alignof(std::max_align_t);
    union {
        void* heap_ptr;
        alignas(BufferAlignment) std::byte buffer[BufferSize];
    } storage;

    bool on_heap;
    const FuncTable* funcs;

public:
    template <typename T>
    Any(const T& value) {
        *this = value;
    }

    template <typename T>
    Any& operator=(const T& value) {
        using DecayedT = std::decay_t<T>;
        if (sizeof(DecayedT) <= BufferSize &&
            alignof(DecayedT) <= BufferAlignment) {
            new (&storage.buffer) DecayedT(value);
            on_heap = false;
        } else {
            storage.heap_ptr = new DecayedT(value);
            on_heap = true;
        }

        funcs = &Handler<DecayedT>::functions;
        return *this;
    }

    ~Any() {
        if (on_heap) {
            funcs->destroy(storage.heap_ptr);
            delete static_cast<char*>(storage.heap_ptr);
        } else {
            funcs->destroy(&storage.buffer);
        }
    }

    Any(const Any& other) : funcs(other.funcs), on_heap(other.on_heap) {
        if (on_heap) {
            storage.heap_ptr = funcs->copy(nullptr, other.storage.heap_ptr);
        } else {
            funcs->copy(&storage.buffer, &other.storage.buffer);
        }
    }

    const std::type_info& type() const {
        return funcs->type();
    }

    decltype(auto) get() const {
        return storage;
    }
    template <typename T>
    friend T* any_cast(Any* any);
};

template <typename T>
const Any::FuncTable Any::Handler<T>::functions = {
    &Any::Handler<T>::destroy, &Any::Handler<T>::copy, &Any::Handler<T>::type};

template <typename T>
T* any_cast(Any* any) {
    if (any && any->type() == typeid(T)) {
        if (any->on_heap) {
            return static_cast<T*>(any->storage.heap_ptr);
        } else {
            return std::launder(reinterpret_cast<T*>(any->storage.buffer));
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
    if (!ptr)
        throw std::bad_cast();
    return *ptr;
}
