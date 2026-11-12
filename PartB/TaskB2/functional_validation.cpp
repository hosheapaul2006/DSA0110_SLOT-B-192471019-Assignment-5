#include "validation.hpp"
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
