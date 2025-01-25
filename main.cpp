#include <iostream>
#include "any/any.hpp"

int main() {
    Any a = 42;                     // Store an int (uses SBO)
    std::cout << any_cast<int>(a) << "\n";
    a = 10;
    std::cout << any_cast<int>(a) << "\n";
}
