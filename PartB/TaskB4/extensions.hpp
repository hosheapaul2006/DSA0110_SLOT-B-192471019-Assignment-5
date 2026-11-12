#ifndef HOSPITAL_EXTENSION_CLASSES_HPP
#define HOSPITAL_EXTENSION_CLASSES_HPP
#include "../TaskB1/payers.hpp"
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
