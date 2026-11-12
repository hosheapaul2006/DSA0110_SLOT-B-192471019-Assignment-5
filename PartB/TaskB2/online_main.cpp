#ifndef HOSPITAL_PAYERS_HPP
#define HOSPITAL_PAYERS_HPP
#ifndef HOSPITAL_BILLING_HPP
#define HOSPITAL_BILLING_HPP
#ifndef HOSPITAL_ERRORS_HPP
#define HOSPITAL_ERRORS_HPP
#include <stdexcept>
namespace hospital {
class BillingError : public std::runtime_error { public: using std::runtime_error::runtime_error; };
class ValidationError : public BillingError { public: using BillingError::BillingError; };
class ClaimError : public BillingError { public: using BillingError::BillingError; };
class PackageError : public BillingError { public: using BillingError::BillingError; };
class MissingPatientError : public ValidationError { public: using ValidationError::ValidationError; };
}
#endif
#include <cmath>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace hospital {
inline constexpr double maximumAmount = 1e12;
inline double roundPaise(double amount) noexcept { return std::round(amount * 100.0) / 100.0; }
inline double checkedAmount(double amount) {
    if (!std::isfinite(amount) || amount < 0 || amount > maximumAmount)
        throw ValidationError("charge must be finite and between 0 and 1e12 rupees");
    return roundPaise(amount);
}

class Patient {
    int id_;
    std::string name_;
public:
    Patient(int id, std::string name) : id_(id), name_(std::move(name)) {
        if (id <= 0 || name_.empty()) throw ValidationError("missing patient identity");
    }
    int id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
};
class PatientRegistry {
    std::map<int, Patient> patients_;
public:
    void add(Patient patient) {
        const int id = patient.id();
        if (!patients_.emplace(id, std::move(patient)).second) throw ValidationError("duplicate patient record");
    }
    const Patient& require(int id) const {
        auto found = patients_.find(id);
        if (found == patients_.end()) throw MissingPatientError("missing patient record");
        return found->second;
    }
};

class Bill {
    int id_;
    Patient patient_;
    std::string packageCode_;
public:
    inline static int live = 0;
    Bill(int id, Patient patient, std::string packageCode)
        : id_(id), patient_(std::move(patient)), packageCode_(std::move(packageCode)) {
        if (id <= 0) throw ValidationError("bill ID must be positive");
        if (patient_.id() <= 0 || patient_.name().empty()) throw ValidationError("missing patient identity");
        ++live;
    }
    Bill(const Bill&) = delete; // Prevent by-value slicing of a polymorphic bill.
    Bill& operator=(const Bill&) = delete;
    Bill(Bill&& other) noexcept
        : id_(other.id_), patient_(std::move(other.patient_)), packageCode_(std::move(other.packageCode_)) { ++live; }
    Bill& operator=(Bill&&) = delete;
    virtual ~Bill() noexcept { --live; }
    int id() const noexcept { return id_; }
    const Patient& patient() const noexcept { return patient_; }
    const std::string& packageCode() const noexcept { return packageCode_; }
    virtual double total() const noexcept = 0;
    virtual const char* type() const noexcept = 0;
    void print(std::ostream& out) const { out << type() << ' ' << patient_.name() << " bill=" << id_ << " total=" << total(); }
};

class OutPatientBill : public Bill {
    double consultation_, tests_;
public:
    OutPatientBill(int id, Patient patient, std::string package, double consultation, double tests)
        : Bill(id, std::move(patient), std::move(package)), consultation_(checkedAmount(consultation)), tests_(checkedAmount(tests)) {
        (void)checkedAmount(total());
    }
    double total() const noexcept override { return roundPaise(consultation_ + tests_); }
    const char* type() const noexcept override { return "OP"; }
};
class InPatientBill : public Bill {
    int days_;
    double perDay_, procedures_, medicines_;
public:
    InPatientBill(int id, Patient patient, std::string package, int days, double perDay, double procedures, double medicines)
        : Bill(id, std::move(patient), std::move(package)), days_(days), perDay_(checkedAmount(perDay)),
          procedures_(checkedAmount(procedures)), medicines_(checkedAmount(medicines)) {
        if (days <= 0) throw ValidationError("inpatient days must be positive");
        (void)checkedAmount(total());
    }
    double total() const noexcept override { return roundPaise(days_ * perDay_ + procedures_ + medicines_); }
    const char* type() const noexcept override { return "IP"; }
};
class DayCareBill : public Bill {
    double packagePrice_;
public:
    DayCareBill(int id, Patient patient, std::string package, double packagePrice)
        : Bill(id, std::move(patient), std::move(package)), packagePrice_(checkedAmount(packagePrice)) {}
    double total() const noexcept override { return packagePrice_; }
    const char* type() const noexcept override { return "DayCare"; }
};
class EmergencyBill : public Bill {
    double base_, hours_, hourlyRate_;
public:
    EmergencyBill(int id, Patient patient, std::string package, double base, double hours, double hourlyRate)
        : Bill(id, std::move(patient), std::move(package)), base_(checkedAmount(base)), hours_(hours), hourlyRate_(checkedAmount(hourlyRate)) {
        if (!std::isfinite(hours) || hours < 0) throw ValidationError("hours must be finite and nonnegative");
        (void)checkedAmount(total());
    }
    double total() const noexcept override { return roundPaise(base_ + hours_ * hourlyRate_); }
    const char* type() const noexcept override { return "Emergency"; }
};

class Ledger {
    std::vector<std::unique_ptr<Bill>> bills_;
public:
    std::size_t size() const noexcept { return bills_.size(); }
    const Bill* find(int id) const noexcept {
        for (const auto& bill : bills_) if (bill->id() == id) return bill.get();
        return nullptr;
    }
    const Bill& add(std::unique_ptr<Bill> bill) {
        if (!bill) throw ValidationError("null bill owner");
        if (find(bill->id())) throw ValidationError("duplicate bill ID");
        bills_.push_back(std::move(bill));
        return *bills_.back();
    }
};
inline double dailyTotal(const Bill* const* bills, std::size_t count) {
    if (count && !bills) throw ValidationError("missing bill array");
    double sum = 0;
    for (std::size_t i = 0; i < count; ++i) {
        if (!bills[i]) throw ValidationError("missing bill pointer");
        sum = checkedAmount(sum + bills[i]->total());
    }
    return sum;
}
}
#endif
#include <algorithm>
#include <ostream>

namespace hospital {
class Payer {
public:
    virtual ~Payer() noexcept = default;
    virtual double amountCovered(const Bill& bill) const = 0;
    virtual const char* name() const noexcept = 0;
};
class SelfPay : public Payer {
public:
    double amountCovered(const Bill&) const override { return 0; }
    const char* name() const noexcept override { return "SelfPay"; }
};
class Insurance : public Payer {
    double limit_, coPayment_;
public:
    Insurance(double limit, double coPayment) : limit_(checkedAmount(limit)), coPayment_(coPayment) {
        if (!std::isfinite(coPayment) || coPayment < 0 || coPayment > 100)
            throw ValidationError("co-payment percentage must be 0..100");
    }
    double amountCovered(const Bill& bill) const override {
        double proposed = roundPaise(bill.total() * ((100 - coPayment_) / 100));
        if (proposed > limit_) throw ClaimError("exceeds cover");
        return proposed;
    }
    const char* name() const noexcept override { return "Insurance"; }
};
using PackageTable = std::map<std::string, double>;
inline PackageTable samplePackages() {
    return {{"OP_STD", 1500}, {"IP_STD", 40000}, {"DC_STD", 20000}, {"ER_STD", 10000}};
}
class GovernmentScheme : public Payer {
    PackageTable packages_;
public:
    explicit GovernmentScheme(PackageTable packages) : packages_(std::move(packages)) {
        for (auto& item : packages_) {
            if (item.first.empty()) throw ValidationError("empty package code");
            item.second = checkedAmount(item.second);
        }
    }
    double amountCovered(const Bill& bill) const override {
        auto found = packages_.find(bill.packageCode());
        if (found == packages_.end()) throw PackageError("unknown package");
        return std::min(bill.total(), found->second);
    }
    const char* name() const noexcept override { return "Government"; }
};
struct Settlement { double total, covered, patient; };
inline Settlement settle(const Bill& bill, const Payer& payer) {
    double covered = payer.amountCovered(bill);
    if (!std::isfinite(covered) || covered < 0 || covered > bill.total()) throw ValidationError("invalid payer contribution");
    covered = checkedAmount(covered);
    return {bill.total(), covered, roundPaise(bill.total() - covered)};
}
inline Settlement settleWithAudit(const Bill& bill, const Payer& payer, std::ostream& audit) {
    try { return settle(bill, payer); }
    catch (const BillingError& error) {
        try { audit << "audit bill=" << bill.id() << ": " << error.what() << '\n'; }
        catch (...) { /* best effort: do not replace the active billing exception */ }
        throw;
    }
}
}
#endif
#ifndef CONTROLLED_ALLOCATION_PROBE_HPP
#define CONTROLLED_ALLOCATION_PROBE_HPP
#include <cstddef>
#include <new>
// Test-only injector. When enabled, OS allocation is deliberately NOT attempted.
class AllocationProbe {
public:
    inline static bool failNext = false;
    inline static int live = 0, constructed = 0, destroyed = 0;
    static void* operator new(std::size_t bytes) {
        if (failNext) { failNext = false; throw std::bad_alloc(); }
        return ::operator new(bytes);
    }
    static void operator delete(void* pointer) noexcept { ::operator delete(pointer); }
    AllocationProbe() noexcept { ++live; ++constructed; }
    ~AllocationProbe() noexcept { --live; ++destroyed; }
};
class FailNextProbeAllocation {
    bool previous_;
public:
    FailNextProbeAllocation() noexcept : previous_(AllocationProbe::failNext) { AllocationProbe::failNext = true; }
    ~FailNextProbeAllocation() noexcept { AllocationProbe::failNext = previous_; }
    FailNextProbeAllocation(const FailNextProbeAllocation&) = delete;
};
#endif
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
