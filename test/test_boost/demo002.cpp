#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const int n = 1000;
    std::vector<int> data(n);

    // 初始化数组
    for (int i = 0; i < n; ++i) {
        data[i] = i + 1;
    }

    int sum = 0;

#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < n; ++i) {
        sum += data[i];
    }

    std::cout << "Sum: " << sum << std::endl;

    return 0;
}
