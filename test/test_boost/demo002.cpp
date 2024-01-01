template <typename Derived>
class Base {
public:
    void foo() {
        static_cast<Derived *>(this)->fooImpl(); // 调用派生类的实现
    }
};

class Derived : public Base<Derived> {
public:
    void fooImpl() {
        // 派生类的具体实现
    }
};

int main() {
    Derived obj;
    obj.foo(); // 调用基类的函数，实际上会调用派生类的实现
    return 0;
}
