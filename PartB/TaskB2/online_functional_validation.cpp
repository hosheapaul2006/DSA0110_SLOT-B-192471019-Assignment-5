#ifndef CLAIM_VALIDATION_EXPERIMENT_HPP
#define CLAIM_VALIDATION_EXPERIMENT_HPP
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
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <vector>

namespace validation_experiment {
inline double approveThrow(double billed, double cover) {
    if (!std::isfinite(billed) || !std::isfinite(cover) || billed<0 || cover<0)
        throw std::invalid_argument("negative or nonfinite amount");
    if (billed>cover) throw hospital::ClaimError("exceeds cover");
    return billed;
}
inline std::optional<double> approveOptional(double billed, double cover) noexcept {
    if (!std::isfinite(billed) || !std::isfinite(cover) || billed<0 || cover<0 || billed>cover) return std::nullopt;
    return billed;
}
enum class Status { Approved, InvalidAmount, ExceedsCover };
struct StatusResult { Status status; double amount; };
inline StatusResult approveStatus(double billed, double cover) noexcept {
    if (!std::isfinite(billed) || !std::isfinite(cover) || billed<0 || cover<0) return {Status::InvalidAmount,0};
    if (billed>cover) return {Status::ExceedsCover,0};
    return {Status::Approved,billed};
}
enum class Method { Optional, Exceptions, ReturnCode };
inline const char* name(Method method) noexcept {
    switch(method) {
        case Method::Optional:return "optional";
        case Method::Exceptions:return "exceptions";
        case Method::ReturnCode:return "return_code";
    }
    return "unknown";
}
inline std::vector<double> makeClaims(std::size_t count, unsigned invalidPercent) {
    if(count==0 || count%100!=0 || invalidPercent>100) throw std::invalid_argument("invalid experiment dimensions");
    std::vector<double> values(count);
    for(std::size_t i=0;i<count;++i) values[i]=(i%100<invalidPercent)?60000.0:40000.0;
    std::mt19937 random(192471019u+invalidPercent);
    std::shuffle(values.begin(),values.end(),random); // same seeded input reused by every method
    return values;
}
struct Result { double paid=0; std::size_t rejected=0; };
inline Result process(Method method, const std::vector<double>& claims) {
    Result result;
    switch(method) {
        case Method::Optional:
            for(double amount:claims) {
                auto paid=approveOptional(amount,50000);
                if(paid) result.paid+=*paid; else ++result.rejected;
            }
            break;
        case Method::Exceptions:
            for(double amount:claims) {
                try { result.paid+=approveThrow(amount,50000); }
                catch(const hospital::ClaimError&) { ++result.rejected; }
            }
            break;
        case Method::ReturnCode:
            for(double amount:claims) {
                auto paid=approveStatus(amount,50000);
                if(paid.status==Status::Approved) result.paid+=paid.amount; else ++result.rejected;
            }
            break;
    }
    return result;
}
inline volatile double observableChecksum=0;
inline void benchmark(Tests& tests, const std::vector<Method>& methods) {
    constexpr std::size_t count=1000000;
    constexpr int trials=5;
    if(methods.empty()) throw std::invalid_argument("no benchmark methods");
    std::cout << "trial,method,claims,invalid_percent,elapsed_ms,rejected,approved_sum\n" << std::fixed << std::setprecision(6);
    for(unsigned percentage:{0u,1u,50u}) {
        const auto claims=makeClaims(count,percentage);
        const std::size_t expectedRejected=count*percentage/100;
        const double expectedPaid=(count-expectedRejected)*40000.0;
        for(int trial=1;trial<=trials;++trial) {
            for(std::size_t turn=0;turn<methods.size();++turn) {
                const Method method=methods[(turn+static_cast<std::size_t>(trial-1))%methods.size()];
                const auto begin=std::chrono::steady_clock::now();
                Result result=process(method,claims);
                observableChecksum=result.paid+static_cast<double>(result.rejected); // one store, not one per claim
                const auto end=std::chrono::steady_clock::now();
                tests.check(result.rejected==expectedRejected,"benchmark exact rejection count");
                tests.check(result.paid==expectedPaid,"benchmark independent approved-sum checksum");
                std::cout << trial << ',' << name(method) << ',' << count << ',' << percentage << ','
                          << std::chrono::duration<double,std::milli>(end-begin).count() << ','
                          << result.rejected << ',' << result.paid << '\n';
            }
        }
    }
}
}
#endif
#include <limits>
int main(int argc, char** argv) {
    using namespace validation_experiment;
    Tests tests(argc,argv);
    std::cout << std::fixed << std::setprecision(2);
    for(unsigned percentage:{0u,1u,50u}) {
        const auto values=makeClaims(1000,percentage);
        const std::size_t rejected=10*percentage;
        const double expectedPaid=(1000-rejected)*40000.0;
        for(Method method:{Method::Optional,Method::Exceptions,Method::ReturnCode}) {
            auto result=process(method,values);
            tests.check(result.rejected==rejected,"functional exact rejection count");
            tests.check(result.paid==expectedPaid,"functional approved amount");
        }
        std::cout << "Invalid " << percentage << "%: rejected=" << rejected << " approved sum=" << expectedPaid << '\n';
    }
    for(double amount:{40000.0,60000.0,-1.0,std::numeric_limits<double>::quiet_NaN(),
                       std::numeric_limits<double>::infinity(),50000.0,0.0}) {
        const bool valid=std::isfinite(amount) && amount>=0 && amount<=50000;
        auto optional=approveOptional(amount,50000);
        auto status=approveStatus(amount,50000);
        bool threw=false;
        double paid=0;
        try {paid=approveThrow(amount,50000);}catch(const std::exception&){threw=true;}
        tests.check(optional.has_value()==valid && (status.status==Status::Approved)==valid && threw==!valid,
                    "three validation designs agree on normal/boundary/invalid input");
        if(valid) tests.check(*optional==amount && status.amount==amount && paid==amount,"three approved values identical");
    }
    tests.check(!approveOptional(1,-1) && approveStatus(1,-1).status==Status::InvalidAmount,"negative cover rejected by no-throw APIs");
    bool negativeCover=false;
    try { (void)approveThrow(1,-1); } catch(const std::invalid_argument&) { negativeCover=true; }
    tests.check(negativeCover,"negative cover rejected by exception API");
    for(int badDimension=0;badDimension<3;++badDimension) {
        bool rejected=false;
        try { (void)makeClaims(badDimension==0?0:badDimension==1?99:1000,badDimension==2?101:0); }
        catch(const std::invalid_argument&) { rejected=true; }
        tests.check(rejected,"invalid experiment dimensions rejected");
    }
    std::cout << "Optional, exceptions and return codes agree on boundary cases\n";
    std::cout << "sizeof optional<double>=" << sizeof(std::optional<double>) << " StatusResult=" << sizeof(StatusResult)
              << " ClaimError=" << sizeof(hospital::ClaimError) << '\n';
}
