#include "billing.hpp"
#include "../../tools/checks.hpp"
#include <iostream>
namespace values {
struct OP {
    double consultation, tests;
    OP(double c, double t) : consultation(hospital::checkedAmount(c)), tests(hospital::checkedAmount(t)) {}
    double total() const { return hospital::checkedAmount(consultation + tests); }
};
struct IP {
    int days; double perDay, procedures, medicines;
    IP(int d, double r, double p, double m) : days(d), perDay(hospital::checkedAmount(r)),
        procedures(hospital::checkedAmount(p)), medicines(hospital::checkedAmount(m)) {
        if (d <= 0) throw hospital::ValidationError("invalid days");
    }
    double total() const { return hospital::checkedAmount(days * perDay + procedures + medicines); }
};
struct DayCare {
    double price;
    explicit DayCare(double p) : price(hospital::checkedAmount(p)) {}
    double total() const { return price; }
};
struct Emergency {
    double base, hours, rate;
    Emergency(double b, double h, double r) : base(hospital::checkedAmount(b)), hours(h), rate(hospital::checkedAmount(r)) {
        if (!std::isfinite(h) || h < 0) throw hospital::ValidationError("invalid hours");
    }
    double total() const { return hospital::checkedAmount(base + hours * rate); }
};
}
template<class T> double groupTotal(const std::vector<T>& bills) {
    double sum = 0;
    for (const T& bill : bills) sum = hospital::checkedAmount(sum + bill.total());
    return sum;
}
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    std::vector<values::OP> op{{500,1200}};
    std::vector<values::IP> ip{{3,2500,40000,2500}};
    std::vector<values::DayCare> dc; dc.emplace_back(25000);
    std::vector<values::Emergency> er{{3000,4,2000}};
    tests.check(groupTotal(op)==1700 && groupTotal(ip)==50000 && groupTotal(dc)==25000 && groupTotal(er)==11000, "four statically selected value-type calculations");
    const double total=groupTotal(op)+groupTotal(ip)+groupTotal(dc)+groupTotal(er);
    tests.check(total==87700, "template groups match independent day total");
    tests.check(groupTotal(std::vector<values::OP>{})==0, "empty typed container");
    bool rejected=false;
    try { values::OP invalid(-1,0); } catch(const hospital::ValidationError&) { rejected=true; }
    tests.check(rejected, "negative static-model component rejected");
    std::cout << "Template separate-container total=" << total << '\n'
              << "sizeof nonvirtual OP=" << sizeof(values::OP) << " production OP=" << sizeof(hospital::OutPatientBill) << '\n';
}
