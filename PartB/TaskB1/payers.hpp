#ifndef HOSPITAL_PAYERS_HPP
#define HOSPITAL_PAYERS_HPP
#include "billing.hpp"
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
