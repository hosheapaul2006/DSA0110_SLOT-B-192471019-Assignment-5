#ifndef EXTENSION_REGRESSION_TESTS_HPP
#define EXTENSION_REGRESSION_TESTS_HPP
#include "extensions.hpp"
#include "../TaskB2/allocation_probe.hpp"
#include "../../tools/checks.hpp"
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
