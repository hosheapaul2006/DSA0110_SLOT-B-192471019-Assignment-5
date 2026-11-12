#include "../../../tools/checks.hpp"
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
