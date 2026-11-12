#ifndef BASELINE_BILLING_TEST_SUITE_HPP
#define BASELINE_BILLING_TEST_SUITE_HPP
#include "payers.hpp"
#include "../../tools/checks.hpp"
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
