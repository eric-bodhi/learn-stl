#include "src/UniquePtr.hpp"

#include <iostream>

int main() {
    UniquePtr<int> p = makeUnique(10);
    UniquePtr<int> x = makeUnique(5);

    std::cout << *p;
    return 0;
}
