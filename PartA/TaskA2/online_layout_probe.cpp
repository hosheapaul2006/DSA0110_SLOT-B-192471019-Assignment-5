#include <iostream>
#ifndef COURSEWORK_CHECKS_HPP
#define COURSEWORK_CHECKS_HPP
#include <iostream>
#include <stdexcept>
#include <string>
class Tests {
    bool logging;
    unsigned count=0;
public:
    Tests(int argc,char** argv):logging(argc>1 && std::string(argv[1])=="--tests") {}
    void check(bool condition,const char* scenario) {
        ++count;
        if(logging) std::cerr << "CHECK|" << scenario << '|' << (condition?"PASS":"FAIL") << '\n';
        if(!condition) throw std::runtime_error(scenario);
    }
    ~Tests() { if(logging) std::cerr << "TEST_COUNT " << count << '\n'; }
};
#endif
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
