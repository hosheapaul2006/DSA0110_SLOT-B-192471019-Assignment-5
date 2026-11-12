#ifndef DISPATCH_EXPERIMENT_MODELS_HPP
#define DISPATCH_EXPERIMENT_MODELS_HPP
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
#include <variant>

namespace dispatch_experiment {
using namespace hospital;
enum class Kind { OP, IP, DayCare, Emergency };
struct Input {
    Kind kind=Kind::OP;
    int days=1;
    double consultation=0,tests=0,perDay=0,procedures=0,medicines=0;
    double packagePrice=0,base=0,hours=0,hourlyRate=0;
};
inline const char* packageFor(Kind kind) {
    switch(kind) {
        case Kind::OP:return "OP_STD";
        case Kind::IP:return "IP_STD";
        case Kind::DayCare:return "DC_STD";
        case Kind::Emergency:return "ER_STD";
    }
    throw ValidationError("unknown input kind");
}
inline Input fixture(std::size_t index) {
    Input result;
    switch(index%10) {
        case 0:result.kind=Kind::OP;result.consultation=500;result.tests=1200;break;
        case 1:result.kind=Kind::IP;result.days=3;result.perDay=2500;result.procedures=40000;result.medicines=2500;break;
        case 2:result.kind=Kind::DayCare;result.packagePrice=25000;break;
        case 3:result.kind=Kind::Emergency;result.base=3000;result.hours=4;result.hourlyRate=2000;break;
        case 4:result.kind=Kind::OP;result.consultation=1000;result.tests=3000;break;
        case 5:result.kind=Kind::IP;result.days=1;result.perDay=1000;result.procedures=5000;result.medicines=1000;break;
        case 6:result.kind=Kind::DayCare;result.packagePrice=10000;break;
        case 7:result.kind=Kind::Emergency;result.base=1000;result.hours=2.5;result.hourlyRate=1000;break;
        case 8:result.kind=Kind::OP;break;
        case 9:result.kind=Kind::IP;result.days=2;result.perDay=2000;result.medicines=500;break;
    }
    const double adjustment=static_cast<double>((index/10)%97);
    switch(result.kind) {
        case Kind::OP:result.consultation+=adjustment;break;
        case Kind::IP:result.medicines+=adjustment;break;
        case Kind::DayCare:result.packagePrice+=adjustment;break;
        case Kind::Emergency:result.base+=adjustment;break;
    }
    return result;
}
inline double independentExpected(std::size_t index) noexcept {
    const double handTotals[]={1700,50000,25000,11000,4000,7000,10000,3500,0,4500};
    return handTotals[index%10]+static_cast<double>((index/10)%97);
}
inline std::unique_ptr<Bill> makeVirtual(int id, const Input& input) {
    Patient patient(1,"Benchmark");
    switch(input.kind) {
        case Kind::OP:return std::make_unique<OutPatientBill>(id,patient,packageFor(input.kind),input.consultation,input.tests);
        case Kind::IP:return std::make_unique<InPatientBill>(id,patient,packageFor(input.kind),input.days,input.perDay,input.procedures,input.medicines);
        case Kind::DayCare:return std::make_unique<DayCareBill>(id,patient,packageFor(input.kind),input.packagePrice);
        case Kind::Emergency:return std::make_unique<EmergencyBill>(id,patient,packageFor(input.kind),input.base,input.hours,input.hourlyRate);
    }
    throw ValidationError("unknown input kind");
}
using Variant=std::variant<OutPatientBill,InPatientBill,DayCareBill,EmergencyBill>;
inline Variant makeVariant(int id, const Input& input) {
    Patient patient(1,"Benchmark");
    switch(input.kind) {
        case Kind::OP:return Variant(std::in_place_type<OutPatientBill>,id,patient,packageFor(input.kind),input.consultation,input.tests);
        case Kind::IP:return Variant(std::in_place_type<InPatientBill>,id,patient,packageFor(input.kind),input.days,input.perDay,input.procedures,input.medicines);
        case Kind::DayCare:return Variant(std::in_place_type<DayCareBill>,id,patient,packageFor(input.kind),input.packagePrice);
        case Kind::Emergency:return Variant(std::in_place_type<EmergencyBill>,id,patient,packageFor(input.kind),input.base,input.hours,input.hourlyRate);
    }
    throw ValidationError("unknown input kind");
}
class TaggedBill {
    int id_;
    Patient patient_;
    std::string package_;
    Input input_;
public:
    TaggedBill(int id, Input input):id_(id),patient_(1,"Benchmark"),package_(packageFor(input.kind)),input_(input) {
        if(id<=0)throw ValidationError("invalid tagged bill ID");
        // Normalize the same individual components as the production constructors.
        switch(input_.kind) {
            case Kind::OP:
                input_.consultation=checkedAmount(input_.consultation);input_.tests=checkedAmount(input_.tests);break;
            case Kind::IP:
                if(input_.days<=0)throw ValidationError("invalid tagged inpatient days");
                input_.perDay=checkedAmount(input_.perDay);input_.procedures=checkedAmount(input_.procedures);
                input_.medicines=checkedAmount(input_.medicines);break;
            case Kind::DayCare:input_.packagePrice=checkedAmount(input_.packagePrice);break;
            case Kind::Emergency:
                if(!std::isfinite(input_.hours) || input_.hours<0)throw ValidationError("invalid tagged emergency hours");
                input_.base=checkedAmount(input_.base);input_.hourlyRate=checkedAmount(input_.hourlyRate);break;
        }
        (void)checkedAmount(total());
    }
    double total() const noexcept {
        switch(input_.kind) {
            case Kind::OP:return roundPaise(input_.consultation+input_.tests);
            case Kind::IP:return roundPaise(input_.days*input_.perDay+input_.procedures+input_.medicines);
            case Kind::DayCare:return input_.packagePrice;
            case Kind::Emergency:return roundPaise(input_.base+input_.hours*input_.hourlyRate);
        }
        return 0; // Unreachable for a constructed object: packageFor rejects invalid kind.
    }
};
inline double visitTotal(const Variant& bill) noexcept {
    return std::visit([](const auto& value) noexcept {return value.total();},bill);
}
enum class Method { Virtual, Switch, VariantVisit };
inline const char* methodName(Method method) noexcept {
    switch(method) {
        case Method::Virtual:return "virtual";
        case Method::Switch:return "switch";
        case Method::VariantVisit:return "variant_visit";
    }
    return "unknown";
}

class Dataset {
    std::vector<std::unique_ptr<Bill>> virtual_;
    std::vector<TaggedBill> tagged_;
    std::vector<Variant> variants_;
    std::vector<double> expected_;
    std::size_t concreteBytes_=0;
public:
    static constexpr std::size_t maximumWorkingSet=65536;
    explicit Dataset(std::size_t count) {
        if(count==0 || count>maximumWorkingSet)throw ValidationError("working set outside bound");
        const auto largest=std::max({sizeof(OutPatientBill),sizeof(InPatientBill),sizeof(DayCareBill),sizeof(EmergencyBill)});
        const std::size_t conservativeBytes=count*(largest+sizeof(std::unique_ptr<Bill>)+sizeof(TaggedBill)+sizeof(Variant)+sizeof(double));
        if(conservativeBytes>64u*1024u*1024u)throw ValidationError("working-set payload budget exceeded");
        virtual_.reserve(count);tagged_.reserve(count);variants_.reserve(count);expected_.reserve(count);
        for(std::size_t i=0;i<count;++i) {
            Input input=fixture(i);
            const int id=static_cast<int>(i+1);
            virtual_.push_back(makeVirtual(id,input));
            tagged_.emplace_back(id,input);
            variants_.push_back(makeVariant(id,input));
            expected_.push_back(independentExpected(i));
            switch(input.kind) {
                case Kind::OP:concreteBytes_+=sizeof(OutPatientBill);break;
                case Kind::IP:concreteBytes_+=sizeof(InPatientBill);break;
                case Kind::DayCare:concreteBytes_+=sizeof(DayCareBill);break;
                case Kind::Emergency:concreteBytes_+=sizeof(EmergencyBill);break;
            }
        }
    }
    std::size_t size() const noexcept {return expected_.size();}
    double at(Method method,std::size_t index) const {
        if(index>=size())throw std::out_of_range("dataset index");
        switch(method) {
            case Method::Virtual:return virtual_[index]->total();
            case Method::Switch:return tagged_[index].total();
            case Method::VariantVisit:return visitTotal(variants_[index]);
        }
        throw ValidationError("unknown dispatch method");
    }
    double expected(std::size_t logicalCount) const noexcept {
        double full=0,partial=0;
        for(std::size_t i=0;i<size();++i) {
            full+=expected_[i];
            if(i<logicalCount%size())partial+=expected_[i];
        }
        return static_cast<double>(logicalCount/size())*full+partial;
    }
    double run(Method method,std::size_t logicalCount) const {
        double sum=0;
        switch(method) {
            case Method::Virtual:
                for(std::size_t remaining=logicalCount;remaining>0;) {
                    const std::size_t count=std::min(remaining,size());
                    for(std::size_t i=0;i<count;++i)sum+=virtual_[i]->total();
                    remaining-=count;
                }
                return sum;
            case Method::Switch:
                for(std::size_t remaining=logicalCount;remaining>0;) {
                    const std::size_t count=std::min(remaining,size());
                    for(std::size_t i=0;i<count;++i)sum+=tagged_[i].total();
                    remaining-=count;
                }
                return sum;
            case Method::VariantVisit:
                for(std::size_t remaining=logicalCount;remaining>0;) {
                    const std::size_t count=std::min(remaining,size());
                    for(std::size_t i=0;i<count;++i)sum+=visitTotal(variants_[i]);
                    remaining-=count;
                }
                return sum;
        }
        throw ValidationError("unknown dispatch method");
    }
    std::size_t requestedPayloadBytes() const noexcept {
        return concreteBytes_+virtual_.capacity()*sizeof(std::unique_ptr<Bill>)+tagged_.capacity()*sizeof(TaggedBill)
               +variants_.capacity()*sizeof(Variant)+expected_.capacity()*sizeof(double);
    }
};
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
#include <iostream>
int main(int argc,char** argv) {
    using namespace dispatch_experiment;
    Tests tests(argc,argv);
    tests.check(Bill::live==0,"no initial dispatch objects");
    {
        Dataset data(10);
        std::cout << "Index Expected Virtual Switch Variant\n";
        for(std::size_t i=0;i<10;++i) {
            const double expected=independentExpected(i);
            tests.check(data.at(Method::Virtual,i)==expected,"ten-bill virtual matches hand calculation");
            tests.check(data.at(Method::Switch,i)==expected,"ten-bill switch matches hand calculation");
            tests.check(data.at(Method::VariantVisit,i)==expected,"ten-bill variant matches hand calculation");
            std::cout << i << ' ' << expected << ' ' << data.at(Method::Virtual,i) << ' '
                      << data.at(Method::Switch,i) << ' ' << data.at(Method::VariantVisit,i) << '\n';
        }
        tests.check(data.expected(10)==116700,"independent ten-bill sum");
        for(Method method:{Method::Virtual,Method::Switch,Method::VariantVisit}) {
            tests.check(data.run(method,10)==116700,"each design totals ten bills");
            tests.check(data.run(method,23)==data.expected(23),"full/partial replay batches preserve checksum");
            tests.check(data.run(method,0)==0,"zero logical records");
        }
        bool rejected=false;
        try {(void)data.at(Method::Virtual,10);}catch(const std::out_of_range&){rejected=true;}
        tests.check(rejected,"dataset index bounds");
        rejected=false;
        try {(void)data.run(static_cast<Method>(99),1);}catch(const ValidationError&){rejected=true;}
        tests.check(rejected,"invalid dispatch tag");
        std::cout << "Ten-bill total=116700; requested payload bytes=" << data.requestedPayloadBytes() << '\n';
    }
    tests.check(Bill::live==0,"all virtual and variant objects destroyed");
    for(std::size_t invalid:{0u,65537u}) {
        bool rejected=false;
        try {Dataset data(invalid);}catch(const ValidationError&){rejected=true;}
        tests.check(rejected,"bounded working-set guard");
    }
    Input invalid=fixture(0);invalid.consultation=-1;
    bool rejected=false;
    try {TaggedBill bill(1,invalid);}catch(const ValidationError&){rejected=true;}
    tests.check(rejected,"invalid tagged charge rejected");
    invalid.kind=static_cast<Kind>(99);
    rejected=false;
    try {(void)makeVirtual(1,invalid);}catch(const ValidationError&){rejected=true;}
    tests.check(rejected,"unknown bill type rejected");
    const double roundedExpected[]={0.22,0.44,0.11,0.28};
    for(int kind=0;kind<4;++kind) {
        Input fractional;
        fractional.kind=static_cast<Kind>(kind);
        fractional.consultation=0.105;fractional.tests=0.105;
        fractional.days=2;fractional.perDay=0.105;fractional.procedures=0.105;fractional.medicines=0.105;
        fractional.packagePrice=0.105;fractional.base=0.105;fractional.hours=1.5;fractional.hourlyRate=0.105;
        auto virtualBill=makeVirtual(99,fractional);
        TaggedBill tagged(99,fractional);
        Variant variant=makeVariant(99,fractional);
        tests.check(virtualBill->total()==roundedExpected[kind] && tagged.total()==roundedExpected[kind]
                    && visitTotal(variant)==roundedExpected[kind],"all designs normalize individual paise components identically");
    }
    tests.check(Bill::live==0,"fractional-model objects cleaned");
    std::cout << "sizeof Bill=" << sizeof(Bill) << " OP=" << sizeof(OutPatientBill) << " IP=" << sizeof(InPatientBill)
              << " DayCare=" << sizeof(DayCareBill) << " Emergency=" << sizeof(EmergencyBill) << '\n'
              << "sizeof TaggedBill=" << sizeof(TaggedBill) << " Variant=" << sizeof(Variant)
              << " owner pointer=" << sizeof(std::unique_ptr<Bill>) << '\n';
}
