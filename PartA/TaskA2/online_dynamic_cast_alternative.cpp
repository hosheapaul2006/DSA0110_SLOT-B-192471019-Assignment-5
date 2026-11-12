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
#include <stdexcept>
struct Bill {
    virtual ~Bill() = default; // RTTI still needs a polymorphic base.
    double total() const { return 0; }
};
struct OutPatientBill : Bill { double total() const { return 500; } };
struct InPatientBill : Bill { double total() const { return 1000; } };
double manuallyTotal(const Bill& bill) {
    if (const auto* op = dynamic_cast<const OutPatientBill*>(&bill)) return op->total();
    if (const auto* ip = dynamic_cast<const InPatientBill*>(&bill)) return ip->total();
    throw std::invalid_argument("Unsupported bill type");
}
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    OutPatientBill op;
    InPatientBill ip;
    Bill* bills[] = {&op, &ip};
    const double expected[] = {500, 1000};
    for (int i = 0; i < 2; ++i) {
        tests.check(bills[i]->total() == 0 && manuallyTotal(*bills[i]) == expected[i], "manual RTTI selects concrete nonvirtual total");
        std::cout << "Base nonvirtual=" << bills[i]->total() << " manual=" << manuallyTotal(*bills[i]) << '\n';
    }
    Bill unknown;
    bool rejected = false;
    try { (void)manuallyTotal(unknown); } catch (const std::invalid_argument&) { rejected = true; }
    tests.check(rejected, "unregistered type rejected");
}
