#ifndef BASELINE_BILLING_TEST_SUITE_HPP
#define BASELINE_BILLING_TEST_SUITE_HPP
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
#include <iomanip>
#include <limits>
#include <sstream>
#include <type_traits>

inline void runBillingTests(Tests& tests, std::ostream& out) {
    using namespace hospital;
    static_assert(noexcept(std::declval<const Bill&>().total()), "validated total is noexcept");
    static_assert(std::has_virtual_destructor<Bill>::value, "polymorphic destruction");
    static_assert(!std::is_copy_constructible<OutPatientBill>::value, "prevent slicing/copy ownership");
    auto rejects = [&](auto operation, const char* scenario) {
        bool caught = false;
        try { operation(); } catch (const BillingError&) { caught = true; }
        tests.check(caught, scenario);
    };
    tests.check(Bill::live == 0, "baseline begins without live bills");
    {
        PatientRegistry registry;
        registry.add(Patient(1, "Ravi")); registry.add(Patient(2, "Meena"));
        registry.add(Patient(3, "Kavya")); registry.add(Patient(4, "Arun"));
        Ledger ledger;
        const Bill* bills[] = {
            &ledger.add(std::make_unique<OutPatientBill>(1, registry.require(1), "OP_STD", 500, 1200)),
            &ledger.add(std::make_unique<InPatientBill>(2, registry.require(2), "IP_STD", 3, 2500, 40000, 2500)),
            &ledger.add(std::make_unique<DayCareBill>(3, registry.require(3), "DC_STD", 25000)),
            &ledger.add(std::make_unique<EmergencyBill>(4, registry.require(4), "ER_STD", 3000, 4, 2000))
        };
        SelfPay self;
        Insurance insurance(50000, 20);
        GovernmentScheme government(samplePackages());
        const Payer* payers[] = {&self, &insurance, &government};
        const double expectedTotal[] = {1700, 50000, 25000, 11000};
        const double expectedCovered[][3] = {{0,1360,1500},{0,40000,40000},{0,20000,20000},{0,8800,10000}};
        const double expectedPatient[][3] = {{1700,340,200},{50000,10000,10000},{25000,5000,5000},{11000,2200,1000}};
        out << "Type Payer Total Covered Patient\n";
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 3; ++j) {
                Settlement result = settle(*bills[i], *payers[j]);
                tests.check(result.total == expectedTotal[i], "matrix: bill total");
                tests.check(result.covered == expectedCovered[i][j], "matrix: payer coverage");
                tests.check(result.patient == expectedPatient[i][j], "matrix: patient contribution");
                tests.check(result.covered + result.patient == result.total, "matrix: total conserved");
                out << bills[i]->type() << ' ' << payers[j]->name() << ' ' << result.total
                    << ' ' << result.covered << ' ' << result.patient << '\n';
            }
        }
        tests.check(dailyTotal(bills, 4) == 87700, "independent hand-derived daily total");
        tests.check(dailyTotal(nullptr, 0) == 0, "empty daily pointer range");
        rejects([&] { (void)dailyTotal(nullptr, 1); }, "missing nonempty pointer array");
        const Bill* missing[] = {nullptr};
        rejects([&] { (void)dailyTotal(missing, 1); }, "null bill element");
        rejects([&] { (void)registry.require(999); }, "missing patient record");
        rejects([&] { registry.add(Patient(1, "Replacement")); }, "duplicate patient record");
        tests.check(registry.require(1).name() == "Ravi", "duplicate does not overwrite existing patient");
        rejects([&] { ledger.add(nullptr); }, "null bill owner");
        rejects([&] { ledger.add(std::make_unique<DayCareBill>(1, registry.require(1), "DC_STD", 10)); }, "duplicate bill ID");
        tests.check(ledger.size() == 4 && Bill::live == 4, "failed additions retain ledger and release candidate");
        Insurance limited(1000, 20);
        std::ostringstream audit;
        const Bill* retained = ledger.find(2);
        bool rejected = false;
        try { (void)settleWithAudit(*retained, limited, audit); } catch (const ClaimError&) { rejected = true; }
        tests.check(rejected && audit.str() == "audit bill=2: exceeds cover\n", "claim logged and same typed exception rethrown");
        tests.check(ledger.find(2) == retained && ledger.size() == 4 && retained->total() == 50000, "rejected bill remains owned and unchanged");
        Settlement recovered = settle(*retained, self);
        tests.check(recovered.covered == 0 && recovered.patient == 50000, "real recovery by different valid payer");
        out << audit.str() << "Recovered retained bill=2 via SelfPay patient=" << recovered.patient << '\n';
        std::ostringstream brokenLog;
        brokenLog.exceptions(std::ios::badbit);
        try { brokenLog.setstate(std::ios::badbit); } catch (const std::ios_base::failure&) {}
        rejected = false;
        try { (void)settleWithAudit(*retained, limited, brokenLog); } catch (const ClaimError&) { rejected = true; }
        tests.check(rejected, "logging failure does not replace original ClaimError");
        OutPatientBill cheap(50, registry.require(1), "OP_STD", 100, 0);
        tests.check(government.amountCovered(cheap) == 100, "government allowance capped by actual total");
        DayCareBill unknown(51, registry.require(1), "UNKNOWN", 100);
        rejects([&] { (void)government.amountCovered(unknown); }, "unknown government package rejected");
        tests.check(self.amountCovered(unknown) == 0 && unknown.total() == 100, "package rejection is payer-specific and retains bill");
        DayCareBill boundary(52, registry.require(1), "DC_STD", 62500);
        tests.check(insurance.amountCovered(boundary) == 50000, "insurance exact cover boundary after co-payment");
        DayCareBill over(53, registry.require(1), "DC_STD", 62501);
        rejects([&] { (void)insurance.amountCovered(over); }, "insurer share exceeding cover rejected");
        tests.check(Insurance(0,100).amountCovered(boundary) == 0, "one-hundred-percent co-payment");
        tests.check(Insurance(62500,0).amountCovered(boundary) == 62500, "zero co-payment");
        tests.check(OutPatientBill(54, registry.require(1), "OP_STD", 0, 0).total() == 0, "zero charge components");
        tests.check(EmergencyBill(55, registry.require(1), "ER_STD", 100, 0, 200).total() == 100, "zero emergency hours");
        tests.check(EmergencyBill(56, registry.require(1), "ER_STD", 100, 1.5, 200).total() == 400, "fractional emergency hours");
        tests.check(std::abs(OutPatientBill(57, registry.require(1), "OP_STD", 0.105, 0).total()-0.11)<1e-9, "nearest-paise normalization");
        rejects([&] { OutPatientBill b(60, registry.require(1), "OP_STD", -1, 0); }, "negative outpatient charge");
        rejects([&] { InPatientBill b(61, registry.require(1), "IP_STD", 1, 1, 1, -1); }, "negative medicine charge");
        rejects([&] { DayCareBill b(62, registry.require(1), "DC_STD", -1); }, "negative package price");
        rejects([&] { EmergencyBill b(63, registry.require(1), "ER_STD", -1, 1, 1); }, "negative emergency base");
        rejects([&] { InPatientBill b(64, registry.require(1), "IP_STD", 0, 1, 0, 0); }, "nonpositive inpatient days");
        rejects([&] { EmergencyBill b(65, registry.require(1), "ER_STD", 1, -1, 1); }, "negative hours");
        rejects([&] { OutPatientBill b(66, registry.require(1), "OP_STD", std::numeric_limits<double>::quiet_NaN(), 0); }, "NaN charge");
        rejects([&] { EmergencyBill b(67, registry.require(1), "ER_STD", 0, std::numeric_limits<double>::infinity(), 1); }, "infinite hours");
        rejects([&] { OutPatientBill b(68, registry.require(1), "OP_STD", 7e11, 7e11); }, "sum above monetary range");
        rejects([&] { InPatientBill b(69, registry.require(1), "IP_STD", std::numeric_limits<int>::max(), 1e12, 0, 0); }, "room product above range");
        rejects([&] { EmergencyBill b(70, registry.require(1), "ER_STD", 0, 1e308, 1000); }, "overflowing time product");
        rejects([&] { DayCareBill b(0, registry.require(1), "DC_STD", 1); }, "invalid bill ID");
        rejects([] { Patient p(0,"Name"); }, "invalid patient ID");
        rejects([] { Patient p(1,""); }, "empty patient name");
        rejects([] { Insurance p(-1,20); }, "negative cover");
        rejects([] { Insurance p(1,-1); }, "negative co-payment");
        rejects([] { Insurance p(1,101); }, "co-payment above one hundred");
        rejects([] { GovernmentScheme p({{"",1}}); }, "empty package code");
        rejects([] { GovernmentScheme p({{"BAD",-1}}); }, "invalid government allowance");
        class BadPayer : public Payer {
            double amount_;
        public:
            explicit BadPayer(double amount) : amount_(amount) {}
            double amountCovered(const Bill&) const override { return amount_; }
            const char* name() const noexcept override { return "BadPayer"; }
        };
        rejects([&] { (void)settle(cheap, BadPayer(-1)); }, "negative payer result rejected");
        rejects([&] { (void)settle(cheap, BadPayer(101)); }, "overpayment result rejected");
        rejects([&] { (void)settle(cheap, BadPayer(std::numeric_limits<double>::quiet_NaN())); }, "NaN payer result rejected");
        DayCareBill max1(80, registry.require(1), "DC_STD", 1e12), max2(81, registry.require(1), "DC_STD", 1e12);
        const Bill* maxBills[] = {&max1, &max2};
        rejects([&] { (void)dailyTotal(maxBills, 2); }, "day sum above representable demonstration range");
    }
    tests.check(hospital::Bill::live == 0, "complete virtual cleanup after baseline and failed constructions");
}
#endif
#ifndef EXTENSION_REGRESSION_TESTS_HPP
#define EXTENSION_REGRESSION_TESTS_HPP
#ifndef HOSPITAL_EXTENSION_CLASSES_HPP
#define HOSPITAL_EXTENSION_CLASSES_HPP
namespace hospital {
class DiagnosticBill : public Bill {
    double scanFee_, reportFee_;
public:
    DiagnosticBill(int id,Patient patient,std::string package,double scanFee,double reportFee)
        : Bill(id,std::move(patient),std::move(package)),scanFee_(checkedAmount(scanFee)),reportFee_(checkedAmount(reportFee)) {
        (void)checkedAmount(total());
    }
    double total() const noexcept override {return roundPaise(scanFee_+reportFee_);}
    const char* type() const noexcept override {return "Diagnostic";}
};
class CorporatePayer : public Payer {
    double percentage_,cap_;
public:
    CorporatePayer(double percentage,double cap):percentage_(percentage),cap_(checkedAmount(cap)) {
        if(!std::isfinite(percentage) || percentage<0 || percentage>100)
            throw ValidationError("corporate percentage must be 0..100");
    }
    double amountCovered(const Bill& bill) const override {
        return std::min(roundPaise(bill.total()*(percentage_/100)),cap_);
    }
    const char* name() const noexcept override {return "Corporate";}
};
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
#include <sstream>
inline void runExtensionTests(Tests& tests,std::ostream& out) {
    using namespace hospital;
    tests.check(Bill::live==0,"extension begins without surviving earlier objects");
    {
        Patient patient(5,"Leela");
        Ledger ledger;
        const Bill& diagnostic=ledger.add(std::make_unique<DiagnosticBill>(101,patient,"DX_STD",6000,500));
        CorporatePayer corporate(80,100000);
        Insurance oldInsurance(50000,20);
        Settlement result=settle(diagnostic,corporate);
        tests.check(std::string(diagnostic.type())=="Diagnostic" && result.total==6500,"new bill type and independent total");
        tests.check(result.covered==5200 && result.patient==1300,"new payer independently expected coverage/patient share");
        tests.check(settle(diagnostic,oldInsurance).covered==5200,"new bill accepted by unchanged old payer");
        DayCareBill oldBill(102,patient,"DC_STD",250000);
        Settlement capped=settle(oldBill,corporate);
        tests.check(capped.covered==100000 && capped.patient==150000,"new payer handles old bill and its cap");
        DayCareBill exactCap(103,patient,"DC_STD",125000);
        tests.check(settle(exactCap,corporate).covered==100000,"corporate exact cap boundary");
        DiagnosticBill free(104,patient,"DX_STD",0,0);
        tests.check(settle(free,corporate).covered==0,"zero new bill");
        tests.check(CorporatePayer(0,100000).amountCovered(diagnostic)==0,"zero corporate coverage percentage");
        tests.check(CorporatePayer(100,100000).amountCovered(diagnostic)==6500,"full corporate percentage");
        tests.check(CorporatePayer(80,0).amountCovered(diagnostic)==0,"zero corporate cap");
        std::ostringstream audit;
        Insurance inadequate(1,20);
        bool rejected=false;
        try {(void)settleWithAudit(diagnostic,inadequate,audit);}catch(const ClaimError&){rejected=true;}
        tests.check(rejected && audit.str()=="audit bill=101: exceeds cover\n","extended run throws/logs/catches ClaimError");
        tests.check(ledger.find(101)==&diagnostic && ledger.size()==1 && diagnostic.total()==6500,"rejection retains new bill and charge");
        GovernmentScheme existingGovernment(samplePackages());
        rejected=false;
        try {(void)settle(diagnostic,existingGovernment);}catch(const PackageError&){rejected=true;}
        tests.check(rejected,"new package has no invented government allowance");
        tests.check(settle(diagnostic,corporate).covered==5200,"valid retry after rejected requests");
        auto reject=[&](auto action,const char* label) {
            bool caught=false;
            try {action();}catch(const ValidationError&){caught=true;}
            tests.check(caught,label);
        };
        reject([&]{DiagnosticBill b(110,patient,"DX_STD",-1,500);},"negative new scan fee rejected");
        reject([&]{DiagnosticBill b(111,patient,"DX_STD",6000,-1);},"negative new report fee rejected");
        reject([&]{DiagnosticBill b(112,patient,"DX_STD",7e11,7e11);},"new total beyond monetary bound rejected");
        reject([]{CorporatePayer p(101,100000);},"invalid new payer percentage");
        reject([]{CorporatePayer p(80,-1);},"invalid new payer cap");
        bool allocationCaught=false;
        {
            FailNextProbeAllocation injection;
            try {auto resource=std::make_unique<AllocationProbe>();}
            catch(const std::bad_alloc&){allocationCaught=true;}
        }
        tests.check(allocationCaught && AllocationProbe::live==0 && !AllocationProbe::failNext,
                    "extended run catches injected allocation failure without leaks");
        tests.check(ledger.find(101)==&diagnostic,"allocation failure does not remove existing extended bill");
        out << "Extension Diagnostic total=" << result.total << " Corporate covered=" << result.covered << " patient=" << result.patient << '\n'
            << "Old Insurance on new bill covered=" << oldInsurance.amountCovered(diagnostic) << '\n'
            << "New Corporate on old DayCare total=" << oldBill.total() << " covered=" << capped.covered << " patient=" << capped.patient << '\n'
            << audit.str() << "Unknown government package rejected; bill retained\n"
            << "Retry Corporate covered=" << settle(diagnostic,corporate).covered << '\n';
    }
    tests.check(Bill::live==0 && AllocationProbe::live==0,"all extension owners released after thrown and caught errors");
}
#endif
#include <iostream>
int main(int argc,char** argv) {
    Tests tests(argc,argv);
    std::cout << std::fixed << std::setprecision(2);
    runBillingTests(tests,std::cout); // Exact unchanged baseline suite.
    runExtensionTests(tests,std::cout);
}
