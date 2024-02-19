#include "oneapi/tbb.h"
#include <iostream>

using namespace oneapi::tbb;

int main(int argc, char *argv[]) {
    oneapi::tbb::parallel_for(0, 100, 1, [](auto i) { std::cout << i << std::endl; });

    return 0;
}
