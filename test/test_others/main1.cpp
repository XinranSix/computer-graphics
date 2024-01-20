#include "tools/print.h"
#include <algorithm>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    std::vector<int> v { 1, 3, 4, 5, 5 };

    print(v);

    return 0;
}
