#include "payers.hpp"
#include "../../tools/checks.hpp"
#include <functional>
#include <iostream>
struct InsuranceState { double cover, coPayment; };
double policyFunction(const hospital::Bill& bill, const InsuranceState& state) {
    double amount=hospital::roundPaise(bill.total()*((100-state.coPayment)/100));
    if (amount>state.cover) throw hospital::ClaimError("exceeds cover");
    return amount;
}
int main(int argc, char** argv) {
    using namespace hospital;
    Tests tests(argc, argv);
    Patient patient(1,"Policy demo");
    OutPatientBill bill(1,patient,"OP_STD",500,1200);
    Insurance object(50000,20);
    InsuranceState state{50000,20}; // validated sample state; the function pointer does not own it
    double (*function)(const Bill&,const InsuranceState&)=policyFunction;
    std::function<double(const Bill&)> captured=[state](const Bill& b){return policyFunction(b,state);};
    tests.check(object.amountCovered(bill)==1360 && function(bill,state)==1360 && captured(bill)==1360,
                "abstract/function-pointer/std-function policies agree");
    DayCareBill over(2,patient,"DC_STD",62501);
    int rejected=0;
    try {(void)object.amountCovered(over);}catch(const ClaimError&){++rejected;}
    try {(void)function(over,state);}catch(const ClaimError&){++rejected;}
    try {(void)captured(over);}catch(const ClaimError&){++rejected;}
    tests.check(rejected==3,"three policy forms reject same over-cover share");
    state.cover=1;
    bool functionRejected=false;
    try {(void)function(bill,state);}catch(const ClaimError&){functionRejected=true;}
    tests.check(functionRejected && captured(bill)==1360,"value capture independent of later context mutation");
    std::cout << "Equivalent coverage=" << object.amountCovered(bill) << '\n'
              << "sizeof Payer=" << sizeof(Payer) << " Insurance=" << sizeof(Insurance)
              << " function pointer=" << sizeof(function) << " context=" << sizeof(state)
              << " std::function=" << sizeof(captured) << '\n';
}
