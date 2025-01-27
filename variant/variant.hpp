#include <cstddef>
#include <type_traits>
#include <typeinfo>
#include <new>

template <typename... Ts>
class Variant {
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
    template <typename T> requires (std::is_same_v<T, Ts> || ...)
    Variant(const T& value) {
        *this = value;
    }

    template <typename T> requires (std::is_same_v<T, Ts> || ...)
    Variant& operator=(const T& value) {
        using DecayedT = std::decay_t<T>;
        if (sizeof(DecayedT) <= BufferSize && alignof(DecayedT) <= BufferAlignment) {
            new (&storage.buffer) DecayedT(value);
            on_heap = false;
        } else {
            storage.heap_ptr = new DecayedT(value);
            on_heap = true;
        }
        funcs = &Handler<DecayedT>::functions;
        return *this;
    }

    Variant() = default;

    ~Variant() {
        if (on_heap) {
            funcs->destroy(storage.heap_ptr);
            delete static_cast<char*>(storage.heap_ptr);
        } else {
            funcs->destroy(&storage.buffer);
        }
    }

    decltype(auto) get() const {
        return storage;
    }

    // temp any_cast for testing
    template <typename T>
    friend T* any_cast(Variant* variant) {
        // Check if the stored type matches T
        if (variant->funcs->type() == typeid(T)) {
            if (variant->on_heap) {
                return static_cast<T*>(variant->storage.heap_ptr);
            } else {
                return reinterpret_cast<T*>(variant->storage.buffer);
            }
        }
        return nullptr; // Type mismatch
    }
};

template <typename... Ts>
template <typename T>
const typename Variant<Ts...>::FuncTable Variant<Ts...>::Handler<T>::functions = {
    &Variant<Ts...>::Handler<T>::destroy,
    &Variant<Ts...>::Handler<T>::copy,
    &Variant<Ts...>::Handler<T>::type
};
