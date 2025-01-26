#include <iostream>
#include "any/any.hpp"
#include <vector>

int main() {
    Any a = 42;                     // Store an int (uses SBO)
    std::cout << any_cast<int>(a) << "\n";
    std::string s{"hello"};
    a = s;
    std::vector<int> v{1, 2, 3};
    std::cout << any_cast<std::string>(a) << "\n";
    a = v;
    std::cout << any_cast<std::vector<int>>(a).at(1);
}
