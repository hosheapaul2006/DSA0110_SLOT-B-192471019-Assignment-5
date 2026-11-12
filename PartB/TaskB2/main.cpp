#include "../TaskB1/payers.hpp"
#include "allocation_probe.hpp"
#include "../../tools/checks.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>

class ScopeTrace {
    std::string name_;
public:
    inline static int live = 0;
    explicit ScopeTrace(std::string name) : name_(std::move(name)) {
        std::cout << '+' << name_ << '\n';
        ++live;
    }
    ~ScopeTrace() noexcept {
        --live;
        try { std::cout << '~' << name_ << '\n'; } catch (...) {}
    }
};
double parseCharge(const std::string& input) {
    std::size_t consumed = 0;
    double amount = std::stod(input, &consumed);
    if (consumed != input.size()) throw std::invalid_argument("trailing numeric text");
    return hospital::checkedAmount(amount);
}

int main(int argc, char** argv) {
    using namespace hospital;
    Tests tests(argc, argv);
    auto observes = [&](auto operation, const char* label) {
        bool caught = false;
        try { operation(); } catch (const std::exception& error) {
            caught = true;
            std::cout << label << ": " << error.what() << '\n';
        }
        tests.check(caught, label);
    };
    {
        PatientRegistry registry;
        registry.add(Patient(1,"Error demonstration"));
        Ledger ledger;
        const Bill& retained = ledger.add(std::make_unique<DayCareBill>(1,registry.require(1),"DC_STD",100000));
        observes([&] { OutPatientBill bad(2,registry.require(1),"OP_STD",-1,0); }, "Negative charge");
        Insurance insurance(50000,20);
        observes([&] { (void)settleWithAudit(retained,insurance,std::cout); }, "Over-cover claim");
        GovernmentScheme government(samplePackages());
        DayCareBill unknown(3,registry.require(1),"NO_SUCH_PACKAGE",100);
        observes([&] { (void)government.amountCovered(unknown); }, "Unknown package");
        observes([&] { (void)registry.require(999); }, "Missing patient");
        observes([] { (void)parseCharge("abc"); }, "Invalid numeric text");
        observes([] { (void)parseCharge("1e99999"); }, "Numeric range");
        observes([] { (void)parseCharge("12junk"); }, "Trailing text");
        observes([] { (void)parseCharge("nan"); }, "Nonfinite numeric value");
        bool invalidText=false, outOfRange=false;
        try { (void)parseCharge("abc"); } catch(const std::invalid_argument&) { invalidText=true; }
        try { (void)parseCharge("1e99999"); } catch(const std::out_of_range&) { outOfRange=true; }
        tests.check(invalidText,"stod invalid text has the required invalid_argument type");
        tests.check(outOfRange,"stod numeric overflow has the required out_of_range type");
        tests.check(parseCharge("12.50") == 12.5 && parseCharge("0") == 0, "valid numeric values");
        tests.check(ledger.find(1)==&retained && ledger.size()==1 && retained.total()==100000,
                    "all failures preserve existing bill ownership and total");
        Settlement recovered=settle(retained,SelfPay{});
        tests.check(recovered.patient==100000 && recovered.covered==0,"corrected request recovers with explicit SelfPay");
    }
    tests.check(Bill::live==0,"billing objects cleaned after error demonstration");
    const int before=AllocationProbe::constructed;
    bool allocationCaught=false;
    {
        FailNextProbeAllocation inject;
        try { auto resource=std::make_unique<AllocationProbe>(); }
        catch(const std::bad_alloc& error) {
            allocationCaught=true;
            std::cout << "Injected allocation failure: " << error.what() << " (OS allocation not attempted)\n";
        }
    }
    tests.check(allocationCaught && AllocationProbe::constructed==before && AllocationProbe::live==0,
                "injected bad_alloc occurs before object construction");
    tests.check(!AllocationProbe::failNext,"injector state restored");
    { auto normal=std::make_unique<AllocationProbe>(); tests.check(AllocationProbe::live==1,"small ordinary allocation succeeds"); }
    tests.check(AllocationProbe::live==0,"ordinary owner cleaned");
    int successes=0, standardFailures=0, unknownFailures=0;
    for(int input=0;input<3;++input) {
        try {
            ScopeTrace request("request"+std::to_string(input));
            auto resource=std::make_unique<AllocationProbe>();
            if(input==0) throw 42;
            if(input==1) throw ClaimError("example claim failure");
            ++successes;
        } catch(const std::exception& error) {
            ++standardFailures;
            std::cout << "Standard handler: " << error.what() << '\n';
        } catch(...) {
            ++unknownFailures;
            std::cout << "Catch-all: unknown failure; next request continues\n";
        }
        tests.check(ScopeTrace::live==0 && AllocationProbe::live==0,"unwinding cleans all request resources before next iteration");
    }
    tests.check(successes==1 && standardFailures==1 && unknownFailures==1,"catch-all and standard catches preserve loop progress");
    const char* filename="b2_raii_resource.tmp";
    bool fileExceptionCaught=false;
    try {
        ScopeTrace scope("file-scope");
        auto resource=std::make_unique<AllocationProbe>();
        std::ofstream file(filename);
        if(!file) throw std::runtime_error("cannot create resource test file");
        file << "resource written before throw\n";
        throw ClaimError("file-scope failure");
    } catch(const ClaimError&) { fileExceptionCaught=true; }
    std::string line;
    { std::ifstream file(filename); std::getline(file,line); }
    tests.check(fileExceptionCaught && line=="resource written before throw","file destructor flushes/closes during unwinding");
    tests.check(std::remove(filename)==0,"closed temporary file removed successfully");
    tests.check(ScopeTrace::live==0 && AllocationProbe::live==0 && AllocationProbe::constructed==AllocationProbe::destroyed,
                "all constructed resources destroyed after thrown/caught failures");
    std::cout << "RAII memory and file cleanup verified\n";
}
