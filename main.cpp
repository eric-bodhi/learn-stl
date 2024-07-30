#include "src/UniquePtr.hpp"

#include <iostream>

int main() {
    auto up = UniquePtr<int[], std::default_delete<int[]>>(10);
    up[0] = 42;

    std::cout << up[0] << "\n";
    return 0;
}
