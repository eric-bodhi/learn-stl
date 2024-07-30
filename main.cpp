#include "src/UniquePtr.hpp"

#include <iostream>

int main() {
    UniquePtr<int> p = makeUnique<int>(10);
    UniquePtr<int> x = makeUnique<int>(5);

    p.swap(x);

    std::cout << *p << " " << *x << "\n";

    return 0;
}
