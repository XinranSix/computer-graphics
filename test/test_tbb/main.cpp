#include <iostream>
#include <tbb/parallel_for.h>
#include <vector>

int main() {
    // 设置并行计算的范围
    const int size = 1000;
    std::vector<int> data(size, 0);

    // 使用 onetbb 的 parallel_for 进行并行计算
    tbb::parallel_for(0, size, 1, [&data](int i) { data[i] = i * i; });

    tbb::parallel_for(0, size, 1,
                      [&data](int i) { std::cout << data[i] << " "; });
    std::cout << std::endl;

    return 0;
}
