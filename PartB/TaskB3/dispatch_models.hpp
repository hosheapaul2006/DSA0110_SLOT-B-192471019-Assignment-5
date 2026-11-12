#ifndef DISPATCH_EXPERIMENT_MODELS_HPP
#define DISPATCH_EXPERIMENT_MODELS_HPP
#include "../TaskB1/billing.hpp"
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
