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
#include <memory>
struct Interface {
    virtual double total() const = 0;
    virtual ~Interface() = default;
};
struct Bill : Interface { double total() const = 0; }; // Inherits virtualness; no keyword needed here.
struct OPBill : Bill { double total() const override { return 500; } };
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    std::unique_ptr<Bill> bill = std::make_unique<OPBill>();
    tests.check(bill->total() == 500, "inherited virtual remains virtual without repeated keyword");
    std::cout << "Inherited virtual total=" << bill->total() << '\n';
}
