#include "../../tools/checks.hpp"
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
