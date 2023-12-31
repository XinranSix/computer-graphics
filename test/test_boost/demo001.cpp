#include <iostream>
#include <experimental/generator>

std::experimental::generator<int> coroutine(int n) {
    for (int i = 0; i < n; ++i) {
        co_yield i;
    }
}

int main() {
    for (auto i : coroutine(10))
        std::cout << i << '\n';
}