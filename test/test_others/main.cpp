#include <format>
#include <iostream>
#include <ostream>
#include <vector>

void printOddTimesNum(std::vector<int> arr) {
    int eor {};
    int rightOne {};
    for (auto a : arr) {
        eor ^= a;
    }
    rightOne = eor & (~eor + 1);
    int a = 0;
    int b = 0;
    for (auto c : arr) {
        if ((c & rightOne) == 0) {
            a ^= c;
        }
    }
    b = a ^ eor;
    std::cout << std::format("{} {}\n", a, b);
}

int main(int argc, char *argv[]) {

    int v;    // we want to find the sign of v
    int sign; // the result goes here
    // CHAR_BIT is the number of bits per byte (normally 8).
    sign = -(v < 0); // if v < 0 then -1, else 0.
    // or, to avoid branching on CPUs with flag registers (IA32):
    sign = -(int)((unsigned int)((int)v) >> (sizeof(int) * CHAR_BIT - 1));
    // or, for one less instruction (but not portable):
    sign = v >> (sizeof(int) * CHAR_BIT - 1);

    return 0;
}
