#include <iostream>
#include "../../tools/checks.hpp"
// A safe layout probe of the controlled unsafe variant's class shapes. No bad delete.
struct Bill {
    ~Bill() = default;
    virtual double total() const = 0;
};
struct OPBill : Bill { double total() const override { return 500; } };
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    OPBill object;
    const Bill& view = object;
    tests.check(view.total() == 500, "virtual call remains valid with stack lifetime");
    std::cout << "sizeof Bill=" << sizeof(Bill) << " OPBill=" << sizeof(OPBill)
              << " alignof Bill=" << alignof(Bill) << " OPBill=" << alignof(OPBill) << '\n';
}
